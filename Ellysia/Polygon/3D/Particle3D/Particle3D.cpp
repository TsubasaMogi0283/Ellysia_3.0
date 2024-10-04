#include "Particle3D.h" 
#include <Camera.h>
#include <TextureManager.h>
#include <PipelineManager.h>
#include "DirectXSetup.h"


#include <numbers>
#include <Collision.h>
#include "SrvManager.h"
static uint32_t modelIndex;
static uint32_t descriptorSizeSRV_ = 0u;



//RandomParticle用
///パーティクルだけはvoid型で初期化する
Particle3D* Particle3D::Create(uint32_t modelHandle) {
	Particle3D* particle3D = new Particle3D();

	//初期化の所でやってね、Update,Drawでやるのが好ましいけど凄く重くなった。
	//ブレンドモードの設定
	//Addでやるべきとのこと
	PipelineManager::GetInstance()->GenerateParticle3DPSO();

#pragma region デフォルトの設定 
	particle3D->emitter_.count = 100;
	//0.5秒ごとに発生
	particle3D->emitter_.frequency = 0.5f;
	//発生頻度用の時刻。0.0で初期化
	particle3D->emitter_.frequencyTime = 0.0f;
	//SRT
	particle3D->emitter_.transform.scale = { 1.0f,1.0f,1.0f };
	particle3D->emitter_.transform.rotate = { 0.0f,0.0f,0.0f };
	particle3D->emitter_.transform.translate = { 0.0f,0.0f,0.0f };

#pragma endregion

	////マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	particle3D->materialResource_ = DirectXSetup::GetInstance()->CreateBufferResource(sizeof(MaterialData)).Get();


	//テクスチャの読み込み
	particle3D->textureHandle_ = TextureManager::GetInstance()->LoadTexture(ModelManager::GetInstance()->GetModelData(modelHandle).textureFilePath);



	particle3D->InstancingIndex_ = SrvManager::GetInstance()->Allocate();



	//頂点リソースを作る
	particle3D->vertices_ = ModelManager::GetInstance()->GetModelData(modelHandle).vertices;

	particle3D->vertexResource_ = DirectXSetup::GetInstance()->CreateBufferResource(sizeof(VertexData) * particle3D->vertices_.size());

	//読み込みのところでバッファインデックスを作った方がよさそう
	//vertexResourceがnullらしい
	//リソースの先頭のアドレスから使う
	particle3D->vertexBufferView_.BufferLocation = particle3D->vertexResource_->GetGPUVirtualAddress();
	//使用するリソースは頂点のサイズ
	particle3D->vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * particle3D->vertices_.size());
	//１頂点あたりのサイズ
	particle3D->vertexBufferView_.StrideInBytes = sizeof(VertexData);




	//インスタンシング
	particle3D->instancingResource_ = DirectXSetup::GetInstance()->CreateBufferResource(sizeof(ParticleForGPU) * MAX_INSTANCE_NUMBER_);
	descriptorSizeSRV_ =  DirectXSetup::GetInstance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	particle3D->instancingSrvHandleCPU_= SrvManager::GetInstance()->GetCPUDescriptorHandle(particle3D->InstancingIndex_);
	particle3D->instancingSrvHandleGPU_ = SrvManager::GetInstance()->GetGPUDescriptorHandle(particle3D->InstancingIndex_);
	

	SrvManager::GetInstance()->CreateSRVForStructuredBuffer(particle3D->InstancingIndex_, particle3D->instancingResource_.Get(), MAX_INSTANCE_NUMBER_, sizeof(ParticleForGPU));
	
	particle3D->instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&particle3D->instancingData_));

	//Lighting
	particle3D->directionalLightResource_ = DirectXSetup::GetInstance()->CreateBufferResource(sizeof(DirectionalLightData)).Get();
	


	return particle3D;

}




//生成関数
Particle Particle3D::MakeNewParticle(std::mt19937& randomEngine) {
	std::uniform_real_distribution<float> distribute(-1.0f, 1.0f);
	Particle particle;
	particle.transform.scale = { 1.0f,1.0f,1.0f };
	particle.transform.rotate = { 0.0f,0.0f,0.0f };
	//ランダムの値
	Vector3 randomTranslate = { distribute(randomEngine),distribute(randomEngine),distribute(randomEngine) };
	particle.transform.translate = VectorCalculation::Add(emitter_.transform.translate,randomTranslate);
	
	//速度
	std::uniform_real_distribution<float>distVelocity(-1.0f, 1.0f);
	particle.velocity = {distVelocity(randomEngine),distVelocity(randomEngine),distVelocity(randomEngine)};

	//Color
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	particle.color = { distColor(randomEngine),distColor(randomEngine),distColor(randomEngine),1.0f };
	

	//時間
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);
	particle.lifeTime = distTime(randomEngine);
	particle.currentTime = 0;



	return particle;

}

//エミッタ
std::list<Particle> Particle3D::Emission(const Emitter& emmitter, std::mt19937& randomEngine){
	std::list<Particle> particles;

	for (uint32_t count = 0; count < emmitter.count; ++count) {

		particles.push_back(MakeNewParticle(randomEngine));
	}

	return particles;
}


//更新
void Particle3D::Update(Camera& camera){
	

	//C++でいうsrandみたいなやつ
	std::random_device seedGenerator;
	std::mt19937 randomEngine(seedGenerator());
	
	///時間経過
	emitter_.frequencyTime += DELTA_TIME;
	//頻度より大きいなら
	if (emitter_.frequency <= emitter_.frequencyTime) {
		//パーティクルを作る
		particles_.splice(particles_.end(), Emission(emitter_, randomEngine));
		//余計に過ぎた時間も加味して頻度計算する
		emitter_.frequencyTime -= emitter_.frequency;
	}


	//座標の計算など
	numInstance_ = 0;
	for (std::list<Particle>::iterator particleIterator = particles_.begin();
		particleIterator != particles_.end();++particleIterator) {
		if ((*particleIterator).lifeTime <= (*particleIterator).currentTime) {
			
			continue;
		}
		
		//フィールド設定すると風の影響を受ける
		if (isSetField_ == true) {
			if (IsCollisionAABBAndPoint(accelerationField_.area,(*particleIterator).transform.translate)) {
				(*particleIterator).velocity.x += accelerationField_.acceleration.x * DELTA_TIME;
				(*particleIterator).velocity.y += accelerationField_.acceleration.y * DELTA_TIME;
				(*particleIterator).velocity.z += accelerationField_.acceleration.z * DELTA_TIME;
			}
		}

		
		particleIterator->currentTime += DELTA_TIME;
		particleIterator->transform.translate.x += particleIterator->velocity.x * DELTA_TIME;
		particleIterator->transform.translate.y += particleIterator->velocity.y * DELTA_TIME;
		particleIterator->transform.translate.z += particleIterator->velocity.z * DELTA_TIME;
		

		//ビルボード有り
		if (isBillBordMode_ == true) {
			//Y軸でπ/2回転
			//これからはM_PIじゃなくてstd::numbers::pi_vを使おうね
			Matrix4x4 backToFrontMatrix = Matrix4x4Calculation::MakeRotateYMatrix(std::numbers::pi_v<float>);

			//カメラの回転を適用する
			Matrix4x4 billBoardMatrix = Matrix4x4Calculation::Multiply(backToFrontMatrix, camera.worldMatrix_);
			//平行成分はいらないよ
			billBoardMatrix.m[3][0] = 0.0f;
			billBoardMatrix.m[3][1] = 0.0f;
			billBoardMatrix.m[3][2] = 0.0f;

			Matrix4x4 scaleMatrix = Matrix4x4Calculation::MakeScaleMatrix(particleIterator->transform.scale);
			Matrix4x4 translateMatrix = Matrix4x4Calculation::MakeTranslateMatrix(particleIterator->transform.translate);
			

			//パーティクル個別のRotateは関係ないよ
			Matrix4x4 worldMatrix = Matrix4x4Calculation::Multiply(scaleMatrix,Matrix4x4Calculation::Multiply(billBoardMatrix,translateMatrix));
			
			//最大値を超えて描画しないようにする
			if (numInstance_ < MAX_INSTANCE_NUMBER_) {
				instancingData_[numInstance_].World = worldMatrix;
				instancingData_[numInstance_].color = particleIterator->color;

				//アルファはVector4でいうwだね
				float alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime);
				instancingData_[numInstance_].color.w=alpha;

				++numInstance_;
			}
		}
		//ビルボード無し
		else if (isBillBordMode_ == false) {
			//ビルボードやらない版
			Matrix4x4 worldMatrix = Matrix4x4Calculation::MakeAffineMatrix(
				particleIterator->transform.scale,
				particleIterator->transform.rotate,
				particleIterator->transform.translate);
			
			//WVP行列を作成
			
			//最大値を超えて描画しないようにする
			if (numInstance_ < MAX_INSTANCE_NUMBER_) {
				instancingData_[numInstance_].World = worldMatrix;
				instancingData_[numInstance_].color = particleIterator->color;

				//アルファはVector4でいうwだね
				float alpha = 1.0f - (particleIterator->currentTime / particleIterator->lifeTime);
				instancingData_[numInstance_].color.w = alpha;

				++numInstance_;
			}
		}
	}
}

//描画
void Particle3D::Draw(uint32_t textureHandle,Camera& camera){
	
	//更新
	Update(camera);



#pragma region マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	//reinterpret_cast...char* から int* へ、One_class* から Unrelated_class* へなどの変換に使用
	MaterialData* materialData_ = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = materialColor_;
	materialData_->lightingKinds = isEnableLighting_;

	materialData_->uvTransform = Matrix4x4Calculation::MakeIdentity4x4();

	materialResource_->Unmap(0, nullptr);

#pragma endregion

#pragma region DirectionalLight
	
	directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
	directionalLightData_->color = directionalLightColor_;                                                                                                                                                                                          
	directionalLightData_->direction = lightingDirection_;
	directionalLightData_->intensity = directionalLightIntensity_;
	directionalLightResource_->Unmap(0, nullptr);

#pragma endregion

	//コマンドを積む
	DirectXSetup::GetInstance()->GetCommandList()->SetGraphicsRootSignature(PipelineManager::GetInstance()->GetParticle3DRootSignature().Get());
	DirectXSetup::GetInstance()->GetCommandList()->SetPipelineState(PipelineManager::GetInstance()->GetParticle3DGraphicsPipelineState().Get());


	
	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	DirectXSetup::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	DirectXSetup::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//CBVを設定する
	//マテリアル
	DirectXSetup::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	//インスタンシング

	//DirectXSetup::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU_);

	SrvManager::GetInstance()->SetGraphicsRootDescriptorTable(1, InstancingIndex_);


	//SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である
	if (textureHandle_!= 0) {
		TextureManager::GraphicsCommand(2,textureHandle );
	}
	
	//DirectionalLight
	DirectXSetup::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());

	//カメラ
	DirectXSetup::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(4, camera.bufferResource_->GetGPUVirtualAddress());
	

	
	
	//DrawCall
	DirectXSetup::GetInstance()->GetCommandList()->DrawInstanced(UINT(vertices_.size()), numInstance_, 0, 0);
}
