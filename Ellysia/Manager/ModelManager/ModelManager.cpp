#include "Modelmanager.h"
#include <cassert>


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <ReadNode.h>

#include "Matrix4x4Calculation.h"

static uint32_t modelhandle;

ModelManager* ModelManager::GetInstance() {
	//関数内static変数として宣言する
	static ModelManager instance;
	return &instance;
}
ModelData ModelManager::LoadFileFotLeveldata(const std::string& fileNameFolder, const std::string& fileName) {
	ModelData modelData;


	//assimpを利用してしてオブジェクトファイルを読んでいく
	Assimp::Importer importer;
	std::string filePath = fileNameFolder + "/" + fileName + "/" + fileName + ".obj";
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	//メッシュがないのは対応しない
	//後読み込みが出来なかったらここで止まる
	assert(scene->HasMeshes());


	//ファイルを読み、ModelDataを構築していく


	//Meshを解析
	//Meshは複数のFaceで構成され、そのFaceは複数の頂点で構成されている
	//さらにSceneには複数のMeshが存在しているというわけであるらしい
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		//Normalなので法線がない時は止める
		assert(mesh->HasNormals());
		//TextureCoordsなのでTexCoordが無い時は止める
		assert(mesh->HasTextureCoords(0));

		//faceを解析する
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			//三角形のみサポート
			assert(face.mNumIndices == 3);
			//ここからFaceの中身であるVertexの解析を行っていく
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				aiVector3D& position = mesh->mVertices[vertexIndex];
				aiVector3D& normal = mesh->mNormals[vertexIndex];
				aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
				VertexData vertex;
				vertex.position = { position.x,position.y,position.z,1.0f };
				vertex.normal = { normal.x,normal.y,normal.z };
				vertex.texCoord = { texcoord.x,texcoord.y };
				//aiProcess_MakeLeftHandedはz*=-1で、
				vertex.position.x *= -1.0f;
				vertex.normal.x *= -1.0f;
				modelData.vertices.push_back(vertex);



			}


		}

	}


	//Materialを解析する
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			//画像ファイルのパスを保存
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.textureFilePath = fileNameFolder + "/" + fileName + "/" + textureFilePath.C_Str();

		}
	}

	//ノードの読み込み
	modelData.rootNode = ReadNode::GetInstance()->Read(scene->mRootNode);

	//ModelDataを返す
	return modelData;
}

uint32_t ModelManager::LoadModelFileForLevelData(const std::string& directoryPath, const std::string& fileName) {
	//一度読み込んだものはその値を返す
	//新規は勿論読み込みをする
	for (uint32_t i = 0; i < MODEL_MAX_AMOUNT_; i++) {
		//同じモデルを探す
		if (ModelManager::GetInstance()->modelInfromtion_[i].directoryPath == directoryPath &&
			ModelManager::GetInstance()->modelInfromtion_[i].folderName == fileName &&
			ModelManager::GetInstance()->modelInfromtion_[i].filePath == fileName) {
			return ModelManager::GetInstance()->modelInfromtion_[i].handle;
		}
	}

	modelhandle++;

	//モデルの読み込み
	ModelData newModelData = ModelManager::GetInstance()->LoadFileFotLeveldata(directoryPath, fileName);

	//新規登録
	ModelManager::GetInstance()->modelInfromtion_[modelhandle].modelData = newModelData;
	ModelManager::GetInstance()->modelInfromtion_[modelhandle].animationData = {};
	ModelManager::GetInstance()->modelInfromtion_[modelhandle].directoryPath = directoryPath;
	ModelManager::GetInstance()->modelInfromtion_[modelhandle].folderName = fileName;
	ModelManager::GetInstance()->modelInfromtion_[modelhandle].filePath = fileName;
	ModelManager::GetInstance()->modelInfromtion_[modelhandle].handle = modelhandle;

	//値を返す
	return modelhandle;
}


//モデルデータの読み込み
ModelData ModelManager::LoadFile(const std::string& directoryPath, const std::string& fileName) {
	//1.中で必要となる変数の宣言
	ModelData modelData;
	//assimpでモデルを読む
	//assimpを利用してしてモデルファイルを読んでいく
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + fileName;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	//メッシュがないのは対応しない
	assert(scene->HasMeshes());
	//3.実際にファイルを読み、ModelDataを構築していく
	//getline...streamから1行読んでstringに格納する
	//istringstream...文字列を分解しながら読むためのクラス、空白を区切りとして読む
	//objファイルの先頭にはその行の意味を示す識別子(identifier/id)が置かれているので、最初にこの識別子を読み込む
	//Meshを解析
	//Meshは複数のFaceで構成され、そのFaceは複数の頂点で構成されている
	//さらにSceneには複数のMeshが存在しているというわけであるらしい
	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		//Normalなので法線がない時は止める
		assert(mesh->HasNormals());
		//TextureCoordsなのでTexCoordが無い時は止める
		assert(mesh->HasTextureCoords(0));
		//頂点を解析する
		//最初に頂点数分のメモリを確保しておく
		modelData.vertices.resize(mesh->mNumVertices);
		for (uint32_t verticesIndex = 0; verticesIndex < mesh->mNumVertices; ++verticesIndex) {
			aiVector3D& position = mesh->mVertices[verticesIndex];
			aiVector3D& normal = mesh->mNormals[verticesIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][verticesIndex];
			//右手から左手への変換
			modelData.vertices[verticesIndex].position = { -position.x,position.y,position.z,1.0f };
			modelData.vertices[verticesIndex].normal = { -normal.x,normal.y,normal.z };
			modelData.vertices[verticesIndex].texCoord = { texcoord.x,texcoord.y };



		}
		//Indexの解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			//三角形で
			assert(face.mNumIndices == 3);
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData.indices.push_back(vertexIndex);
			}
		}

		//SkinCluster構築用のデータ取得を追加
		for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
			//Jointごとの格納領域を作る
			aiBone* bone = mesh->mBones[boneIndex];
			std::string jointName = bone->mName.C_Str();
			JointWeightData& jointWeightData = modelData.skinClusterData[jointName];

			//InverseBindPoseMatrixの抽出
			aiMatrix4x4 bindPoseMatrixAssimp = bone->mOffsetMatrix.Inverse();
			aiVector3D scale;
			aiVector3D translate;
			aiQuaternion rotate;

			bindPoseMatrixAssimp.Decompose(scale, rotate, translate);

			Vector3 scaleAfter = { scale.x,scale.y,scale.z };
			Vector3 translateAfter = { -translate.x,translate.y,translate.z };
			Quaternion rotateQuaternion = { rotate.x,-rotate.y,-rotate.z,rotate.w };

			Matrix4x4 scaleMatrix = Matrix4x4Calculation::MakeScaleMatrix(scaleAfter);
			Matrix4x4 rotateMatrix = MakeRotateMatrix(rotateQuaternion);
			Matrix4x4 translateMatrix = Matrix4x4Calculation::MakeTranslateMatrix(translateAfter);


			
			Matrix4x4 bindPoseMatrix = Matrix4x4Calculation::Multiply(scaleMatrix, Matrix4x4Calculation::Multiply(rotateMatrix, translateMatrix));
			jointWeightData.inverseBindPoseMatrix = Matrix4x4Calculation::Inverse(bindPoseMatrix);

			//Weight情報を取り出す
			for (uint32_t weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex) {
				jointWeightData.vertexWeights.push_back({ bone->mWeights[weightIndex].mWeight, bone->mWeights[weightIndex].mVertexId });
			}


		}

	}



	//Materialを解析する
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.textureFilePath = directoryPath + "/" + textureFilePath.C_Str();
		}
	}
	//ノードの読み込み
	modelData.rootNode = ReadNode::GetInstance()->Read(scene->mRootNode);
	//ModelDataを返す
	return modelData;
}

uint32_t ModelManager::LoadModelFile(const std::string& directoryPath, const std::string& fileName) {

	//一度読み込んだものはその値を返す
	//新規は勿論読み込みをする
	for (uint32_t i = 0; i < MODEL_MAX_AMOUNT_; i++) {
		//同じモデルを探す
		if (ModelManager::GetInstance()->modelInfromtion_[i].directoryPath == directoryPath &&
			ModelManager::GetInstance()->modelInfromtion_[i].filePath == fileName) {
			return ModelManager::GetInstance()->modelInfromtion_[i].handle;
		}
	}

	modelhandle++;

	//モデルの読み込み
	ModelData newModelData = ModelManager::GetInstance()->LoadFile(directoryPath, fileName);

	//新規登録
	ModelManager::GetInstance()->modelInfromtion_[modelhandle].modelData = newModelData;
	ModelManager::GetInstance()->modelInfromtion_[modelhandle].animationData = {};
	ModelManager::GetInstance()->modelInfromtion_[modelhandle].directoryPath = directoryPath;
	ModelManager::GetInstance()->modelInfromtion_[modelhandle].filePath = fileName;
	ModelManager::GetInstance()->modelInfromtion_[modelhandle].handle = modelhandle;

	//値を返す
	return modelhandle;
}

uint32_t ModelManager::LoadModelFile(const std::string& directoryPath, const std::string& fileName, bool isAnimationLoad) {
	//一度読み込んだものはその値を返す
	//新規は勿論読み込みをする
	for (uint32_t i = 0; i < MODEL_MAX_AMOUNT_; i++) {
		//同じモデルを探す
		if (ModelManager::GetInstance()->modelInfromtion_[i].directoryPath == directoryPath &&
			ModelManager::GetInstance()->modelInfromtion_[i].filePath == fileName) {
			return ModelManager::GetInstance()->modelInfromtion_[i].handle;
		}
	}
	
	modelhandle++;

	//モデルの読み込み
	ModelData newModelData = ModelManager::GetInstance()->LoadFile(directoryPath, fileName);

	Animation newAnimation = {};
	if (isAnimationLoad == true) {
		//アニメーションの読み込み
		newAnimation = LoadAnimationFile(directoryPath, fileName);

	}
	
	//新規登録
	ModelManager::GetInstance()->modelInfromtion_[modelhandle].modelData = newModelData;
	ModelManager::GetInstance()->modelInfromtion_[modelhandle].animationData = newAnimation;
	ModelManager::GetInstance()->modelInfromtion_[modelhandle].directoryPath = directoryPath;
	ModelManager::GetInstance()->modelInfromtion_[modelhandle].filePath = fileName;
	ModelManager::GetInstance()->modelInfromtion_[modelhandle].handle = modelhandle;

	//値を返す
	return modelhandle;
}
