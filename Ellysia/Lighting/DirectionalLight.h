#pragma once
/**
 * @file DirectionalLight.h
 * @brief 平行光源の構造体
 * @author 茂木翼
 */

#include "Vector4.h" 
#include "Vector3.h"
#include "DirectXSetup.h"

/// <summary>
/// 平行光源データ
/// </summary>
struct DirectionalLightData {
	//ライトの色
	Vector4 color;
	//ライトの向き
	Vector3 direction;
	//輝度
	float intensity;
};


/// <summary>
/// 平行光源
/// </summary>
struct DirectionalLight {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();


public:
	//ライトの色
	Vector4 color_ = {1.0f,1.0f,1.0f,1.0f};
	//ライトの向き
	Vector3 direction_ = {0.0f,-1.0f,0.0f};
	//輝度
	float intensity_=5.0f;

	//定数バッファ
	ComPtr<ID3D12Resource> bufferResource_=nullptr;


private:

	//書き込みデータ
	DirectionalLightData* directionalLightData_ = nullptr;

};
