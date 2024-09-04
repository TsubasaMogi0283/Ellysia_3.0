#pragma once
#include "Vector3.h"
#include "WorldTransform.h"
#include "Model.h"
#include "../Collider/Collider.h"
#include "Material.h"
#include <memory>
#include "../../Ellysia/Line/Line.h"
#include "Stage/Ground/StageRect.h"
#include "EnemyAttackCollision.h"
#include "AABB.h"

struct Camera;
struct SpotLight;
class Player;

//後々StatePatternで分けるつもり
enum EnemyCondition {
	//何もしない
	NoneMove,
	//通常
	Move,
	//追いかけ始めるときの座標
	PreTracking,
	//追いかける
	Tracking,
	//攻撃
	Attack,

	//速度反転
	InverseSpeed,


};

class Enemy :public Collider{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Enemy()=default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelHandle"></param>
	/// <param name="position"></param>
	/// <param name="speed"></param>
	void Initialize(uint32_t modelHandle, Vector3 position, Vector3 speed);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();


	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="spotLight"></param>
	void Draw(Camera& camera,SpotLight& spotLight);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy();



public:
	/// <summary>
	/// 衝突判定
	/// </summary>
	void OnCollision()override;



	/// <summary>
	/// ワールド座標
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition()override;

#pragma region 座標の設定





	/// <summary>
	/// Z座標の設定
	/// </summary>
	/// <param name="posX"></param>
	inline void SetPositionZ(float& posZ) {
		this->worldTransform_.translate_.z = posZ;
	}

#pragma endregion



	/// <summary>
	/// 生きているかのフラグを取得
	/// </summary>
	/// <returns></returns>
	inline bool GetIsAlive() const{
		return isAlive_;
	}

	/// <summary>
	/// 向きを取得
	/// </summary>
	/// <returns></returns>
	inline Vector3 GetDirection() const {
		return direction_;
	}

	/// <summary>
	/// プレイヤーの座標を設定
	/// </summary>
	/// <param name="position"></param>
	void SetPlayerPosition(Vector3& position) {
		this->playerPosition_ = position;
	}

	inline AABB GetAABB() {
		return aabb_;
	}



public:
	
	/// <summary>
	/// 前の状態の設定
	/// </summary>
	/// <param name="condition"></param>
	inline void SetPreCondition(uint32_t& preCondition) {
		this->preCondition_ = preCondition;
	}

	uint32_t GetPreCondition() const {
		return preCondition_;
	}


	/// <summary>
	/// 状態の設定
	/// </summary>
	/// <param name="condition"></param>
	inline void SetCondition(uint32_t& condition) {
		this->condition_ = condition;
	}

	uint32_t GetCondition() const {
		return condition_;
	}



	inline void InvertSpeedX() {
		speed_.x *= -1.0f;
	}
	inline void InvertSpeedZ() {
		speed_.z *= -1.0f;
	}


	inline void SaveSpeed() {
		preSpeed_ = speed_;
	}


#pragma region 攻撃用

	//攻撃用
	inline EnemyAttackCollision* GetEnemyAttackCollision() {
		return attackModel_;
	}

	/// <summary>
	/// 攻撃しているかどうか
	/// </summary>
	/// <returns></returns>
	inline bool GetIsAttack()const {
		return isAttack_;
	}

#pragma endregion


	



private:
	uint32_t preCondition_ = EnemyCondition::NoneMove;
	uint32_t condition_ = EnemyCondition::Move;



private:
	//ワールドトランスフォーム
	WorldTransform worldTransform_ = {};
	const float SCALE_SIZE = 8.0f;
	//移動速度
	Vector3 preSpeed_ = {};
	Vector3 speed_ = {};

	//モデル
	std::unique_ptr<Model> model_ = nullptr;
	
	//マテリアル
	Material material_ = {};
	Vector4 color_ = {};
	
	//消滅
	int32_t deleteTime_ = 0;
	bool isAlive_ = true;

	//追跡
	bool isTracking_ = false;
	Vector3 preTrackingPosition_ = {};
	Vector3 preTrackingPlayerPosition_ = {};

	//向き
	Vector3 direction_ = {};
	float t_ = 0.0f;

	//攻撃
	int32_t attackTime_ = 0;

	//AABB
	AABB aabb_ = {};


	//プレイヤーの座標
	Vector3 playerPosition_ = {};

	//モデル
	std::unique_ptr<Model> debugModel_ = nullptr;
	WorldTransform debugModelWorldTransform_ = {};

	//攻撃用
	EnemyAttackCollision* attackModel_ = nullptr;
	bool isAttack_ = false;

};