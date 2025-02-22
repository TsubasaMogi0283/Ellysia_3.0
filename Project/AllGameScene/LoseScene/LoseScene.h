#pragma once
/**
 * @file LoseScene.h
 * @brief 負けシーンのクラス
 * @author 茂木翼
 */
#include <memory>

#include "IGameScene.h"
#include "Sprite.h"
#include "Model.h"
#include "Camera.h"
#include "PointLight.h" 
#include "BackTexture.h"
#include "DissolveEffect.h"
#include "Dissolve.h"




/// <summary>
/// EllysiaEngine(前方宣言)
/// </summary>
namespace Ellysia {
	/// <summary>
	/// 入力
	/// </summary>
	class Input;

	/// <summary>
	/// レベルデータ管理クラス
	/// </summary>
	class LevelDataManager;

	/// <summary>
	/// グローバル変数
	/// </summary>
	class GlobalVariables;

	/// <summary>
	/// テクスチャ管理クラス
	/// </summary>
	class TextureManager;

	/// <summary>
	/// ゲーム管理クラス
	/// </summary>
	class GameManager;


	/// <summary>
	/// モデル管理クラス
	/// </summary>
	class ModelManager;


};


/// <summary>
/// 負けシーン
/// </summary>
class LoseScene : public Ellysia::IGameScene {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	LoseScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="gameManager"></param>
	void Update(Ellysia::GameManager* gameManager)override;


	/// <summary>
	/// 3Dオブジェクト
	/// </summary>
	void DrawObject3D()override;

	/// <summary>
	/// ポストエフェクト描画処理前
	/// </summary>
	void PreDrawPostEffectFirst()override;

	/// <summary>
	/// ポストエフェクト描画処理前
	/// </summary>
	void DrawPostEffect()override;


	/// <summary>
	/// スプライト描画
	/// </summary>
	void DrawSprite()override;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~LoseScene()=default;

private:
	/// <summary>
	/// /ImGUiの表示
	/// </summary>
	void DisplayImGui();

	/// <summary>
	/// 調整
	/// </summary>
	void Adjustment();

private:
	//入力クラス
	Ellysia::Input* input_ = nullptr;
	//テクスチャ管理クラス
	Ellysia::TextureManager* textureManager_ = nullptr;
	//レベルデータ管理クラス
	Ellysia::LevelDataManager* levelDataManager_ = nullptr;
	//ハンドル
	uint32_t levelDataHandle_ = 0u;

	//モデル管理クラス
	Ellysia::ModelManager* modelManager_ = nullptr;
	//グローバル変数クラス
	Ellysia::GlobalVariables* globalVariables_ = nullptr;
	
private:
	//調整項目
	//点光源
	const std::string POINT_LIGHT_NAME = "LoseScenePointLight";
	//ディゾルブ
	const std::string DISSOLVE_NAME_ = "LoseSceneDissolve";

	//時間変化
	const float DELTA_TIME = 1.0f / 60.0f;

	//間隔
	const float TRANSPARENCY_INTERVAL_ = 0.01f;
	//点滅どのくらい
	const uint32_t FLASH_TIME_LIMIT_ = 30u;

	//高速点滅どのくらい
	const uint32_t FAST_FLASH_TIME_LIMIT_ = 60u;
	//高速点滅間隔
	const uint32_t FAST_FLASH_TIME_INTERVAL_ = 3u;
	//タイトルに変わる時間
	const uint32_t CHANGE_TO_TITLE_TIME_ = 60 * 1;


	//最大の半径
	const float MAX_LIGHT_RADIUS_ = 11.8f;
	

private:
	//カメラ
	Camera camera_ = {};
	//速度
	Vector3 cameraVelocity_ = {};
	//移動時間
	float cameraMoveTime_ = 0.0f;


	//マテリアル
	Material material_ = {};
	//点光源
	PointLight pointLight_ = {};
	//半径の線形補間
	float lightRadiusT_ = 0.0f;

	//背景(ポストエフェクト)
	std::unique_ptr<Ellysia::BackTexture>backTexture_ = nullptr;
	//ディゾルブ
	std::unique_ptr<Ellysia::DissolvePostEffect> dissolveEffect_ = nullptr;
	Dissolve dissolve_ = {};

	//テキスト
	std::unique_ptr<Ellysia::Sprite> text_ = nullptr;
	//黒背景
	std::unique_ptr<Ellysia::Sprite> black_ = nullptr;
	//透明度
	float transparency_ = 0.0f;
	
	//見せる番号
	uint32_t textDisplayCount_ = 0u;

	//暗転している時間
	uint32_t blackOutTime_ = 0u;

	//Bトリガー
	uint32_t bTriggerTime_ = 0u;
	//フラグ
	bool isBTrigger_ = false;
	//タイトル
	bool isReturnTitle = false;
	float returnToTitleDissolveThresholdT_ = 0.0f;



	//点滅
	bool isFlash_ = false;
	//時間
	uint32_t flashTime_ = 0u;
	

	//高速点滅
	bool isFastFlash_ = false;
	
	//時間
	uint32_t fastFlashTime_ = 0u;

	
	//ライトアップが終わったかどうか
	bool isFinishLightUp_ = false;
	
	//ゲームに戻るかどうか
	bool isReturnToGame_ = false;



};

