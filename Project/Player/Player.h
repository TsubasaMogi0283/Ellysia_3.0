#pragma once
#include "WorldTransform.h"
#include "Model.h"
#include <memory>
#include "Stage/Ground/StageRect.h"
#include "../Collider/Collider.h"
#include "AABB.h"
#include "PlayerCollisionToStrongEnemy.h"

struct Camera;
struct SpotLight;
struct Material;
class SampleScene;
class ObjectManager;

enum PlayerViewOfPoint {
	//未定
	NonePerson = 0,
	//1人称視点
	FirstPerson = 1,
	//3人称後方
	ThirdPersonBack = 2,
};

enum PlayerMoveCondition {
	//動かない
	NonePlayerMove,
	//動く
	OnPlayerMove,
};

class Player :public Collider {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Player() = default;

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
	void Draw(Camera& camera,Material& material,SpotLight& spotLight);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player() = default;

public:




	/// <summary>
	/// ワールド座標を所得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition()override;

	/// <summary>
	///	衝突
	/// </summary>
	void OnCollision()override;
	

	/// <summary>
	/// AABBの取得
	/// </summary>
	inline AABB GetAABB() {
		return aabb_;
	}


	/// <summary>
	/// 半径を取得
	/// </summary>
	/// <returns></returns>
	inline float GetRadius() const {
		return radius_;
	}

	/// <summary>
	/// 持っている鍵の数を増やす
	/// </summary>
	inline void AddHaveKeyQuantity() {
		haveKeyQuantity_++;
	}

	/// <summary>
	/// 今鍵を何個持っているか
	/// </summary>
	/// <returns></returns>
	inline uint32_t GetHavingKey() {
		return haveKeyQuantity_;
	}

	/// <summary>
	/// 動く方向の設定
	/// </summary>
	/// <param name="move"></param>
	inline void SetMoveDirection(Vector3& moveDirection) {
		this->moveDirection_ = moveDirection;
	}

	/// <summary>
	/// 動きの状態を設定
	/// </summary>
	/// <param name="condition"></param>
	inline void SetPlayerMoveCondition(uint32_t& condition) {
		this->moveCondition_ = condition;
	}

	/// <summary>
	/// 走るかどうか
	/// </summary>
	inline void SetIsDash(bool& isDash) {
		this->isDash_ = isDash;
	}

	/// <summary>
	/// 四隅の取得
	/// </summary>
	/// <param name="stageRect"></param>
	/// <returns></returns>
	inline void SetStageRect(StageRect stageRect) {
		this->stageRect_ = stageRect;
	}

	/// <summary>
	/// 操作を受け付けるか受け付けないかの設定
	/// </summary>
	/// <param name="isControll"></param>
	inline void SetIsAbleToControll(bool isControll) {
		this->isControll_ = isControll;
	}

	/// <summary>
	/// 体力を取得
	/// </summary>
	/// <returns></returns>
	uint32_t GetHP()const {
		return hp_;
	}

	

	/// <summary>
	/// 一発アウトの敵用の当たり判定
	/// </summary>
	/// <returns></returns>
	PlayerCollisionToStrongEnemy* GetCollisionToStrongEnemy()const {
		return collisionToStrongEnemy_.get();
	}

private:

	//モデル
	std::unique_ptr<Model> model_ = nullptr;

	//ワールドトランスフォーム
	WorldTransform worldTransform_={};

	//ステージの四隅
	StageRect stageRect_ = {};



	//持っている鍵の数
	//可算なのでQuantity
	uint32_t haveKeyQuantity_ = 0u;

	//動く方向
	Vector3 moveDirection_ = {};

	const float SIDE_SIZE = 1.0f;
	AABB aabb_ = {};

	//体力
	int32_t hp_ = 0;
	//敵の攻撃に当たった時のタイマー
	int32_t downTime_ = 0;
	//敵の攻撃に当たったかどうか
	bool isDamage_ = false;
	bool acceptDamage_ = false;

	//操作可能かどうか
	bool isControll_ = false;
	//移動状態
	uint32_t moveCondition_ = 0u;

	//ダッシュ
	bool isDash_ = false;





	//当たり判定(一発アウトの敵用)
	std::unique_ptr<PlayerCollisionToStrongEnemy>collisionToStrongEnemy_ = nullptr;



};

