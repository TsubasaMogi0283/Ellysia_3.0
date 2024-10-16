#pragma once
#include <string>
#include <list>
#include <array>
#include <map>
#include <memory>
#include <fstream>

#include <Vector3.h>
#include <json.hpp>
#include <Model.h>
#include "WorldTransform.h"
#include "Collider/Collider.h"


struct Camera;
struct Material;
struct DirectionalLight;
struct PointLight;
struct SpotLight;

class LevelDataManager final{
private:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	LevelDataManager() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~LevelDataManager() = default;

public:
	//シングルインスタンス
	static LevelDataManager* GetInstance();

	//コピーコンストラクタ禁止
	LevelDataManager(const LevelDataManager& levelDataManager) = delete;

	//代入演算子を無効にする
	LevelDataManager& operator=(const LevelDataManager& levelDataManager) = delete;


public:

	/// <summary>
	/// レベルデータの読み込み
	/// </summary>
	/// <param name="filePath">jsonファイル</param>
	uint32_t Load(const std::string& filePath);

	/// <summary>
	/// 再読み込み
	/// </summary>
	/// <param name="levelDataHandle"></param>
	void Reload(uint32_t& levelDataHandle);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="levelDataHandle"></param>
	void Update(uint32_t&levelDataHandle);

	/// <summary>
	/// 消去
	/// </summary>
	/// <param name="levelDataHandle"></param>
	void Delete(uint32_t& levelDataHandle);



#pragma region 描画

	/// <summary>
	/// 描画(平行光源)
	/// </summary>
	/// <param name="levelDataHandle"></param>
	/// <param name="camera"></param>
	/// <param name="material"></param>
	/// <param name="directionalLight"></param>
	void Draw(uint32_t& levelDataHandle,Camera& camera, Material& material, DirectionalLight& directionalLight);

	/// <summary>
	/// 描画(点光源)
	/// </summary>
	/// <param name="levelDataHandle"></param>
	/// <param name="camera"></param>
	/// <param name="material"></param>
	/// <param name="pointLight"></param>
	void Draw(uint32_t& levelDataHandle,Camera& camera, Material& material, PointLight& pointLight);

	/// <summary>
	/// 描画(スポットライト)
	/// </summary>
	/// <param name="levelDataHandle"></param>
	/// <param name="camera"></param>
	/// <param name="material"></param>
	/// <param name="spotLight"></param>
	void Draw(uint32_t& levelDataHandle,Camera& camera,Material & material,SpotLight& spotLight);


#pragma endregion

	/// <summary>
	/// デストラクタの代わり
	/// </summary>
	void Release();




private:
	

	struct LevelData {
		struct ObjectData {
			//モデル
			Model* model = nullptr;
			//ワールドトランスフォーム
			WorldTransform* worldTransform = {};

			//オブジェクトのファイル名
			std::string fileName;
			
			//Transform
			Vector3 scaling;
			Vector3 rotation;
			Vector3 translation;


			std::string colliderType;
			//Sphere,Box
			Vector3 center;
			Vector3 size;

			//AABB
			AABB aabb;
			Vector3 upSize;
			Vector3 downSize;


		};


		//ハンドル
		uint32_t handle = 0u;

		//オブジェクト
		std::list<ObjectData> objectDatas = {};

		//フォルダ名
		std::string folderName = {};
		//ファイル名
		std::string fileName = {};
		//フルパス
		std::string fullPath = {};

	};

	std::list<LevelData::ObjectData> GetObject(uint32_t& handle) {
		
		for (const auto& [key, levelData] : levelDatas_) {
			
			//一致したら返す
			if (levelData->handle == handle) {
				return levelData->objectDatas;
			}
		}

		//見つからない
		return {};
	}


private:
	/// <summary>
	/// 位置決め
	/// </summary>
	void Place(nlohmann::json& objects, LevelData& levelData);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="directoryPath"></param>
	void Ganarate(LevelData& levelData);

	
	/// <summary>
	/// JSONファイルを解凍
	/// </summary>
	/// <param name="file"></param>
	/// <param name="fullFilePath"></param>
	/// <returns></returns>
	nlohmann::json Deserialize(std::ifstream& file,std::string& fullFilePath);


private:

	//ここにデータを入れていく
	std::map<std::string , std::unique_ptr<LevelData>> levelDatas_;

	uint32_t handle_ = 0u;


};

