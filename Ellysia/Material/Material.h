#pragma once

/**
 * @file Material.h
 * @brief マテリアル
 * @author 茂木翼
 */



#include <cstdint>

#include "Vector4.h"
#include "Matrix4x4.h"
#include "DirectXSetup.h"
#include "LightingType.h"

/// <summary>
/// データ
/// </summary>
struct MaterialData {
	//色
	Vector4 color;
	//ライティングの種類
	int32_t lightingKinds;
	float padding[6];
	//UVトランスフォーム
	Matrix4x4 uvTransform;
	//輝度
	float shininess;
	//環境マッピングするかどうか
	bool isEnviromentMap;
	//ディゾルブするかどうか
	bool isDissolve;

};

/// <summary>
/// マテリアル
/// </summary>
struct Material {
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

	//色
	Vector4 color_ = {1.0f,1.0f,1.0f,1.0f};
	//Lightingの種類
	int32_t lightingKinds_=Directional;
	//UV行列
	Matrix4x4 uvTransform_ = {};
	//輝度
	float shininess_=100.0f;
	//環境マップ
	bool isEnviromentMap_ = false;
	//ディゾルブ
	bool isDissolve_ = false;

	//定数バッファ
	ComPtr<ID3D12Resource> resource_ = nullptr;
	//書き込みのデータ
	MaterialData* materialData_ = nullptr;

};