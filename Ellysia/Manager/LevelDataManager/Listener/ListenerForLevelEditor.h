#pragma once
#include <Vector3.h>

/// <summary>
/// レベルエディタ用のリスナー
/// </summary>
class ListenerForLevelEditor{
public:
	/// <summary>
	/// ワールド座標の取得
	/// </summary>
	/// <returns></returns>
	virtual Vector3 GetWorldPosition()=0;

	/// <summary>
	/// 向きの取得
	/// </summary>
	/// <returns></returns>
	virtual Vector3 GetDirection() = 0;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ListenerForLevelEditor() = default;

};
