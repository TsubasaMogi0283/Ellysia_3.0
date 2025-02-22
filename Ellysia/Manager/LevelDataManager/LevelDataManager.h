#pragma once

/**
 * @file LevelDataManager.h
 * @brief レベルデータ管理クラス
 * @author 茂木翼
 */

#include <string>
#include <list>
#include <map>
#include <memory>
#include <fstream>
#include <json.hpp>

#include "Vector3.h"
#include "Model.h"
#include "WorldTransform.h"
#include "Collider.h"
#include "Transform.h"
#include "Model/IObjectForLevelEditor.h"
#include "Model/AudioObjectForLevelEditor.h"
#include "Listener.h"

#pragma region 前方宣言

/// <summary>
/// カメラ
/// </summary>
struct Camera;

/// <summary>
/// マテリアル
/// </summary>
struct Material;

/// <summary>
/// 平行光源
/// </summary>
struct DirectionalLight;

/// <summary>
/// 点光源
/// </summary>
struct PointLight;

/// <summary>
/// スポットライト
/// </summary>
struct SpotLight;

#pragma endregion



/// <summary>
/// EllysiaEngine
/// </summary>
namespace Ellysia {

	/// <summary>
	/// レベルデータ管理クラス
	/// </summary>
	class LevelDataManager final {
	private:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		LevelDataManager();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~LevelDataManager() = default;

	public:
		/// <summary>
		/// インスタンスの取得
		/// </summary>
		/// <returns>インスタンス</returns>
		static LevelDataManager* GetInstance();

		/// <summary>
		/// コピーコンストラクタ禁止
		/// </summary>
		/// <param name="levelDataManager">レベル管理クラス</param>
		LevelDataManager(const LevelDataManager& levelDataManager) = delete;

		/// <summary>
		/// 代入演算子を無効にする
		/// </summary>
		/// <param name="levelDataManager">レベル管理クラス</param>
		/// <returns></returns>
		LevelDataManager& operator=(const LevelDataManager& levelDataManager) = delete;

	public:
		/// <summary>
		/// レベルデータの読み込み
		/// </summary>
		/// <param name="filePath">ファイルパス</param>
		/// <returns>ハンドル</returns>
		uint32_t Load(const std::string& filePath);

		/// <summary>
		/// 再読み込み
		/// </summary>
		/// <param name="levelDataHandle">ハンドル</param>
		void Reload(const uint32_t& levelDataHandle);

		/// <summary>
		/// 更新
		/// </summary>
		/// <param name="levelDataHandle">ハンドル</param>
		void Update(const uint32_t& levelDataHandle);

		/// <summary>
		/// 消去
		/// </summary>
		/// <param name="levelDataHandle">ハンドル</param>
		void Delete(const uint32_t& levelDataHandle);

		/// <summary>
		/// 描画(平行光源)
		/// </summary>
		/// <param name="levelDataHandle">ハンドル</param>
		/// <param name="camera">カメラ</param>
		/// <param name="material">マテリアル</param>
		/// <param name="directionalLight">平行光源</param>
		void Draw(const uint32_t& levelDataHandle, const Camera& camera, const DirectionalLight& directionalLight);

		/// <summary>
		/// 描画(点光源)
		/// </summary>
		/// <param name="levelDataHandle">ハンドル</param>
		/// <param name="camera">カメラ</param>
		/// <param name="material">マテリアル</param>
		/// <param name="pointLight">点光源</param>
		void Draw(const uint32_t& levelDataHandle, const Camera& camera,const PointLight& pointLight);

		/// <summary>
		/// 描画(スポットライト)
		/// </summary>
		/// <param name="levelDataHandle">ハンドル</param>
		/// <param name="camera">カメラ</param>
		/// <param name="material">マテリアル</param>
		/// <param name="spotLight">スポットライト</param>
		void Draw(const uint32_t& levelDataHandle, const Camera& camera,const SpotLight& spotLight);

		/// <summary>
		/// 解放
		/// デストラクタの代わり
		/// </summary>
		void Finalize();


	public:

		/// <summary>
		/// データにある分だけColliderを取得する
		/// </summary>
		/// <param name="handle">ハンドル</param>
		/// <returns>持っている分のコライダー</returns>
		inline std::vector<IObjectForLevelEditorCollider*> GetCollider(const uint32_t& handle) {
			std::vector<IObjectForLevelEditorCollider*> colliders = {};

			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {

					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {

						//コライダーを持っている場合、リストに追加
						if (objectData.levelDataObjectCollider != nullptr) {
							colliders.push_back(objectData.levelDataObjectCollider);
						}
					}

					//無駄なループを防ぐ
					break;
				}
			}

			return colliders;
		}

		/// <summary>
		/// オーディオの方のコライダーを取得
		/// </summary>
		/// <param name="handle">ハンドル</param>
		/// <returns> オーディオコライダー</returns>
		inline std::vector<IObjectForLevelEditorCollider*>GetAudioCollider(const uint32_t& handle) {
			std::vector<IObjectForLevelEditorCollider*> colliders = {};

			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {

					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {

						//コライダーを持っている場合、リストに追加
						if (objectData.levelDataObjectCollider != nullptr && objectData.type == "Audio") {
							colliders.push_back(objectData.levelDataObjectCollider);
						}
					}

					//無駄なループを防ぐ
					break;
				}
			}

			return colliders;
		}


		/// <summary>
		/// ステージオブジェクトの座標を取得
		/// </summary>
		/// <param name="handle">ハンドル</param>
		/// <returns>ステージオブジェクトの座標</returns>
		inline std::vector<Vector3> GetStageObjectPositions(const uint32_t& handle) {
			std::vector<Vector3> positions = {};

			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {


					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {

						//Stageだったら追加
						if (objectData.type == "Stage") {
							positions.push_back(objectData.objectForLeveEditor->GetWorldPosition());
						}


					}

					//無駄なループを防ぐ
					break;
				}
			}

			return positions;
		}

		/// <summary>
		/// ステージオブジェクトのAABBを取得
		/// </summary>
		/// <param name="handle">ハンドル</param>
		/// <returns>ステージオブジェクトのAABB</returns>
		inline std::vector<AABB> GetStageObjectAABBs(const uint32_t& handle) {
			std::vector<AABB> aabbs = {};

			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {


					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {

						//Stageだったら追加
						if (objectData.type == "Stage") {
							aabbs.push_back(objectData.objectForLeveEditor->GetAABB());
						}


					}

					//無駄なループを防ぐ
					break;
				}
			}

			return aabbs;
		}

		/// <summary>
		/// コライダーを持っているかどうかの取得
		/// </summary>
		/// <param name="handle">ハンドル</param>
		/// <returns>コライダーを持っているかどうかのフラグ</returns>
		inline std::vector<bool> GetIsHavingColliders(const uint32_t& handle) {
			std::vector<bool> colliders = {};

			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {


					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {

						//コライダーを持っているかどうかのフラグを挿入
						colliders.push_back(objectData.isHavingCollider);

					}

					//無駄なループを防ぐ
					break;
				}
			}

			return colliders;
		}

	private:


		/// <summary>
		/// オブジェクトデータ
		/// </summary>
		struct ObjectData {
			//オブジェクトのタイプ
			//今はステージかオーディオのどちらか
			std::string type;

			//個別の名前
			std::string name;

			//ファイル名
			std::string modelFileName;

			//Transform
			Transform transform;


			//コライダーを持っているかどうか
			bool isHavingCollider = false;

			//Colliderの種類
			std::string colliderType;

			uint32_t colliderTypeNumber;


			//Sphere,Box
			Vector3 center;
			Vector3 size;

			//AABB
			AABB aabb;
			Vector3 upSize;
			Vector3 downSize;


			//非表示設定
			bool isInvisible = false;

			//レベルデータのオーディオ
			AudioDataForLevelEditor levelAudioData;

			//オブジェクト(ステージかオーディオ)
			BaseObjectForLevelEditor* objectForLeveEditor;

			//コライダー
			IObjectForLevelEditorCollider* levelDataObjectCollider;


		};


		/// <summary>
		/// レベルデータ
		/// </summary>
		struct LevelData {
			
			//ハンドル
			uint32_t handle = 0u;

			//オブジェクトのリスト
			std::list<ObjectData> objectDatas;

			//リスナー
			//プレイヤーなどを設定してね
			Listener listener = {};

			//フォルダ名
			std::string folderName;
			//ファイル名
			std::string fileName;
			//フルパス
			std::string fullPath;

		};

		/// <summary>
		/// 指定したオブジェクトの取得
		/// </summary>
		/// <param name="handle">ハンドル</param>
		/// <returns>オブジェクトのリスト</returns>
		inline std::list<ObjectData> GetObject(const uint32_t& handle) {

			for (const auto& [key, levelData] : levelDatas_) {

				//一致したら返す
				if (levelData->handle == handle) {
					return levelData->objectDatas;
				}
			}

			//見つからない
			return {};
		}

	public:
		/// <summary>
		/// リスナーの設定
		/// </summary>
		/// <param name="handle">ハンドル</param>
		/// <param name="listener">リスナー</param>
		inline void SetListener(const uint32_t& handle, const Listener& listener) {
			for (const auto& [key, levelData] : levelDatas_) {

				//一致したら設定
				if (levelData->handle == handle) {
					levelData->listener = listener;
				}
			}
		}

		/// <summary>
		/// 個別のスケールの変更
		/// </summary>
		/// <param name="handle"></param>
		/// <param name="name"></param>
		/// <param name="scale"></param>
		inline void SetScale(const uint32_t& handle,const std::string& name, const Vector3& scale) {

			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {

					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {
						//名前が一致したらスケールの変更
						if (objectData.name == name) {
							objectData.objectForLeveEditor->SetScale(scale);
						}
						

					}

					//無駄なループを防ぐ
					break;
				}
			}

		}

		/// <summary>
		/// 個別の回転を変える
		/// </summary>
		/// <param name="handle"></param>
		/// <param name="name"></param>
		/// <param name="rotate"></param>
		inline void SetRotate(const uint32_t& handle,const std::string& name, const Vector3& rotate) {

			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {

					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {
						//一致したら回転の変更
						if (objectData.name == name) {
							objectData.objectForLeveEditor->SetRotate(rotate);
						}
					}

					//無駄なループを防ぐ
					break;
				}
			}

		}

		/// <summary>
		/// 個別の座標を変更
		/// </summary>
		/// <param name="handle"></param>
		/// <param name="name"></param>
		inline void SetPosition(const uint32_t& handle,const std::string& name,const Vector3& translate) {

			for (const auto& [key, levelData] : levelDatas_) {
				if (levelData->handle == handle) {

					//該当するLevelDataのobjectDatasを検索
					for (auto& objectData : levelData->objectDatas) {
						//一致したら座標の変更
						if (objectData.name == name) {
							objectData.objectForLeveEditor->SetPositione(translate);
						}
					}

					//無駄なループを防ぐ
					break;
				}
			}

		}

	private:


		/// <summary>
		/// 配置
		/// </summary>
		/// <param name="objects">オブジェクト</param>
		/// <param name="levelData">レベルデータ</param>
		void Place(nlohmann::json& objects, LevelData& levelData);

		/// <summary>
		/// 生成
		/// </summary>
		/// <param name="levelData"></param>
		void Ganarate(LevelData& levelData);


		/// <summary>
		/// JSONファイルを解凍
		/// </summary>
		/// <param name="fullFilePath"></param>
		/// <returns></returns>
		nlohmann::json Deserialize(const std::string& fullFilePath);

	private:
		//オーディオ
		Ellysia::Audio* audio_ = nullptr;

		
	private:

		//ここにデータを入れていく
		std::map<std::string, std::unique_ptr<LevelData>> levelDatas_;

		//ハンドル
		uint32_t handle_ = 0u;

		//Resourceにあるレベルデータの場所
		const std::string LEVEL_DATA_PATH_ = "Resources/LevelData/";

	};

};