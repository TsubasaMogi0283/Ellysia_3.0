#pragma once

/**
 * @file Player.h
 * @brief プレイヤーのクラス
 * @author 茂木翼
 */

#include <memory>
#include <list>

#include "WorldTransform.h"
#include "Model.h"
#include "AABB.h"

#include "PlayerCollisionToStrongEnemy.h"
#include "PlayerCollisionToNormalEnemyAttack.h"
#include "PlayerCollisionToAudioObject.h"
#include "Light/FlashLight/FlashLight.h"

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
/// ゲームシーン
/// </summary>
class GameScene;

/// <summary>
/// EllysiaEngine
/// </summary>
namespace Elysia {
	/// <summary>
	/// 入力クラス
	/// </summary>
	class Input;

	/// <summary>
	/// モデル管理クラス
	/// </summary>
	class ModelManager;

	/// <summary>
	/// レベルデータ管理クラス
	/// </summary>
	class LevelDataManager;
};



#pragma endregion

/// <summary>
/// プレイヤーの移動状態
/// </summary>
enum PlayerMoveCondition {
	//動かない
	NonePlayerMove,
	//動く
	OnPlayerMove,
};


/// <summary>
/// プレイヤー
/// </summary>
class Player {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Player();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="material"></param>
	/// <param name="spotLight"></param>
	void DrawObject3D(const Camera& camera,const SpotLight& spotLight);

	/// <summary>
	/// スプライトの描画
	/// </summary>
	void DrawSprite();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

private:
	/// <summary>
	/// ダメージ
	/// </summary>
	void Damaged();

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <summary>
	/// ImGui表示用
	/// </summary>
	void DisplayImGui();

public:

	/// <summary>
	/// 半径の取得
	/// </summary>
	/// <returns>半径</returns>
	inline float GetSideSize()const {
		return SIDE_SIZE;
	}

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	/// <returns>ワールド座標</returns>
	inline Vector3 GetWorldPosition()const {
		return worldTransform_.GetWorldPosition();
	}


	/// <summary>
	/// 方向を取得
	/// </summary>
	/// <returns>方向</returns>
	inline Vector3 GetDirection() const{
		return moveDirection_;
	}

	/// <summary>
	/// AABBの取得
	/// </summary>
	/// <returns>AABB</returns>
	inline AABB GetAABB() const{
		return aabb_;
	}


	/// <summary>
	/// 体力を取得
	/// </summary>
	/// <returns>体力</returns>
	uint32_t GetHP()const {
		return hp_;
	}

	/// <summary>
	/// ダメージを受けたかどうかを取得
	/// </summary>
	/// <returns></returns>
	inline bool GetIsDamaged()const {
		return isDameged_;
	}

	/// <summary>
	/// 懐中電灯を取得
	/// </summary>
	/// <returns>懐中電灯</returns>
	inline FlashLight* GetFlashLight()const {
		return flashLight_.get();
	}

public:

	/// <summary>
	/// 持っている鍵の数を増やす
	/// </summary>
	inline void AddHaveKeyQuantity() {
		++haveKeyQuantity_;
	}

	/// <summary>
	/// 今鍵を何個持っているか
	/// </summary>
	/// <returns>鍵の数</returns>
	inline uint32_t GetHavingKey() {
		return haveKeyQuantity_;
	}

	/// <summary>
	/// 動く方向の設定
	/// </summary>
	/// <param name="move">方向</param>
	inline void SetMoveDirection(const Vector3& moveDirection) {
		this->moveDirection_ = moveDirection;
	}

	/// <summary>
	/// 走るかどうか
	/// </summary>
	/// <param name="isDash"></param>
	inline void SetIsDash(const bool& isDash) {
		this->isDash_ = isDash;
	}


	/// <summary>
	/// 操作を受け付けるか受け付けないかの設定
	/// </summary>
	/// <param name="isControll">操作できるかどうか</param>
	inline void SetIsAbleToControll(const bool& isControll) {
		this->isControll_ = isControll;
	}

	/// <summary>
	/// 通常の敵からの攻撃を受け入れるかどうか
	/// </summary>
	/// <param name="isAccept">受け入れるかどうか</param>
	inline void SetIsAcceptDamegeFromNoemalEnemy(const bool& isAccept) {
		this->isAcceptDamegeFromNoemalEnemy_ = isAccept;
	}

	/// <summary>
	/// 通常の敵の攻撃の受け入れを取得
	/// </summary>
	/// <returns>受け入れ状態</returns>
	bool GetIsAcceptDamegeFromNormalEnemy()const {
		return isAcceptDamegeFromNoemalEnemy_;
	}

	/// <summary>
	/// 座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	inline void SetPosition(Vector3& position) {
		this->worldTransform_.translate = position;
	}

	/// <summary>
	/// レベルデータハンドルの設定
	/// </summary>
	/// <param name="levelHandle"></param>
	inline void SetLevelHandle(const uint32_t& levelHandle) {
		this->levelHandle_ = levelHandle;
	}


public:


	/// <summary>
	/// プレイヤー用のコライダーを取得
	/// </summary>
	/// <returns>コライダー</returns>
	inline std::vector<BasePlayerCollision*> GetColliders()const{
		std::vector<BasePlayerCollision*> colliders;
		for (const auto& collider : colliders_) {
			colliders.push_back(collider.get());
		}
		return colliders;
	};


	/// <summary>
	/// 懐中電灯の当たり判定
	/// </summary>
	/// <returns></returns>
	inline FlashLightCollision* GetFlashLightCollision()const {
		return flashLight_->GetFanCollision();
	}





private:
	//入力クラス
	Elysia::Input* input_ = nullptr;
	//モデル管理クラス
	Elysia::ModelManager* modelManager_ = nullptr;
	//レベルエディタ
	Elysia::LevelDataManager* levelDataManager_ = nullptr;
	//ハンドル
	uint32_t levelHandle_ = 0u;

private:
	//幅のサイズ
	const float_t SIDE_SIZE = 0.5f;
	//時間変化
	const float_t DELTA_TIME = 1.0f / 60.0f;
	//チャージの増える値
	const float_t CHARGE_VALUE_ = 0.1f;


private:

	//モデル
	std::unique_ptr<Elysia::Model> model_ = nullptr;
	//ワールドトランスフォーム
	WorldTransform worldTransform_={};
	//動く方向
	Vector3 moveDirection_ = {};
	//中心座標
	Vector3 playerCenterPosition_ = {};
	//マテリアル
	Material material_ = {};
	//AABB
	AABB aabb_ = {};
	//持っている鍵の数
	uint32_t haveKeyQuantity_ = 0u;

	//体力
	int32_t hp_ = 3;
	//敵の攻撃に当たった時のタイマー
	int32_t downTime_ = 0;
	//操作可能かどうか
	bool isControll_ = false;
	//ダッシュ
	bool isDash_ = false;

	//敵の攻撃に当たったかどうか
	bool isDamage_ = false;
	//攻撃されたか
	bool isDameged_ = false;
	//通常の敵からダメージを受けたかどうか
	bool isAcceptDamegeFromNoemalEnemy_ = false;

	//時間
	float_t vibeTime_ = 0u;

private:
	//当たり判定のリスト
	std::list<std::unique_ptr<BasePlayerCollision>> colliders_ = {};
	//懐中電灯
	std::unique_ptr<FlashLight>flashLight_ = nullptr;

};