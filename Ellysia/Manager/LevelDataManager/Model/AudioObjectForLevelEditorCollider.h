#pragma once

#include "IObjectForLevelEditorCollider.h"

/// <summary>
/// オーディオオブジェクト用の当たり判定
/// </summary>
class AudioObjectForLevelEditorCollider :public IObjectForLevelEditorCollider {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	AudioObjectForLevelEditorCollider() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;


	/// <summary>
	/// 衝突
	/// </summary>
	void OnCollision()override;


	/// <summary>
	/// 非衝突
	/// </summary>
	void OffCollision()override;


	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition()override;





private:





};
