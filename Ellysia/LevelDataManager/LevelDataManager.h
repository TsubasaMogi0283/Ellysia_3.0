#pragma once
#include <string>
#include <Vector3.h>
#include <vector>
#include <array>
#include <map>

#include <Model.h>
#include "WorldTransform.h"

//このクラスを元に継承させた方が良いかも
//Modelは別々のものだから


class LevelDataManager {
public:
	LevelDataManager() {};

	~LevelDataManager();

public:

	/// <summary>
	/// 読み込み
	/// </summary>
	/// <param name="filePath"></param>
	void Load(std::string filePath);


	/// <summary>
	/// 生成し配置する
	/// </summary>
	void Configure();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(Camera& camera);

private:
	struct LevelData {
		struct ObjectData {
			//ファイル名
			std::string fileName;
			//Transform
			Vector3 translation;
			Vector3 rotation;
			Vector3 scaling;

		};

		//オブジェクト
		std::vector<ObjectData> objects;



		//ファイルパス
		std::string filePath;


	};

	//モデルのコンテナ
	std::map<std::string,Model*> models_;

	std::vector<WorldTransform*> worldTransforms_;
	LevelData* levelData = nullptr;
private:


	static const uint32_t LEVEL_DATA_MAX_AMOUNT_ = 128;
	//数の限界があるのでarrayがいいかな
	std::array<LevelData, LEVEL_DATA_MAX_AMOUNT_> levelData_{};






};
