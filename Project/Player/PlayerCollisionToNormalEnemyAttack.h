#pragma once
#include "Collider.h"
#include <WorldTransform.h>
#include <memory>
#include <Material.h>
#include <Model.h>

/// <summary>
/// 通常の敵の攻撃に対しての当たり判定
/// </summary>
class PlayerCollisionToNormalEnemyAttack :public Collider {
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlayerCollisionToNormalEnemyAttack() = default;

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
	void Draw(const Camera& camera,const Material& material,const  SpotLight& spotLight);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerCollisionToNormalEnemyAttack() = default;


public:
	/// <summary>
	/// 敵のワールド座標の設定
	/// </summary>
	/// <param name="position"></param>
	inline void SetPlayerPosition(const Vector3& position) {
		this->playerWorldPosition_ = position;
	}

	/// <summary>
	/// 衝突したかどうか
	/// </summary>
	/// <returns></returns>
	inline bool GetIsTouch() const {
		return isTouch_;
	}

	


	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns></returns>
	inline Vector3 GetWorldPosition()override {
		return worldTransform_.GetWorldPosition();
	};

	/// <summary>
	/// 接触
	/// </summary>
	inline void OnCollision()override {
		isTouch_ = true;
	};

	/// <summary>
	/// 非接触
	/// </summary>
	inline void OffCollision()override {
		isTouch_ = false;
	};


private:
	//モデル
	std::unique_ptr<Model> model_ = nullptr;
	//ワールドトランスフォーム
	WorldTransform worldTransform_ = {};
	//プレイヤーの座標
	Vector3 playerWorldPosition_ = {};


	//通常の敵
	bool isTouch_ = false;

};