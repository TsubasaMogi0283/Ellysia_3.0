#pragma once
/**
 * @file Model.h
 * @brief モデル
 * @author 茂木翼
 */

#include "DirectXSetup.h"

#include "Matrix4x4.h"
#include "Vector4.h"
#include "TransformationMatrix.h"
#include "Matrix4x4Calculation.h"
#include "VertexData.h"
#include "LightingType.h"
#include "ModelData.h"

#pragma region 前方宣言

/// <summary>
/// ワールドトランスフォーム
/// </summary>
struct WorldTransform;

/// <summary>
/// カメラ
/// </summary>
struct Camera;

/// <summary>
/// GPUに送るためのカメラ
/// </summary>
struct CameraForGPU;

/// <summary>
/// マテリアル
/// </summary>
struct Material;

/// <summary>
/// 点光源
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
/// モデル
/// </summary>
class Model {
public:

	//コンストラクタ
	Model() = default;

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="modelHandle">モデルハンドル</param>
	/// <returns></returns>
	static Model* Create(uint32_t modelHandle);

	#pragma region 描画

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <param name="camera">カメラ</param>
	/// <param name="directionalLight">平行光源</param>
	void Draw(const WorldTransform& worldTransform,const Camera& camera,const Material& material,const DirectionalLight& directionalLight);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="worldTransform"></param>
	/// <param name="camera"></param>
	/// <param name="pointLight"></param>
	void Draw(const WorldTransform& worldTransform,const Camera& camera,const Material& material,const PointLight& pointLight);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="worldTransform"></param>
	/// <param name="camera"></param>
	/// <param name="spotLight"></param>
	void Draw(const WorldTransform& worldTransform,const Camera& camera,const Material& material,const SpotLight& spotLight);

#pragma endregion

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Model() = default;




public:


	//アクセッサのまとめ
	void SetBlendMode(int32_t blendmode) {
		blendModeNumber_ = blendmode;
	}



	/// <summary>
	/// 環境マップテクスチャの設定
	/// </summary>
	/// <param name="textureHandle"></param>
	void SetEviromentTexture(uint32_t textureHandle) {
		this->eviromentTextureHandle_ = textureHandle;
	}




private:
	//頂点リソースを作る
	//頂点バッファビューを作成する
	ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	//頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	//インデックス
	ComPtr<ID3D12Resource> indexResource_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};


	//PixelShaderにカメラの座標を送る為の変数
	ComPtr<ID3D12Resource> cameraResource_ = nullptr;
	CameraForGPU* cameraForGPU_ = {};


	//アニメーションを再生するときに使う時間
	float animationTime_ = 0.0f;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	//環境マップ
	uint32_t eviromentTextureHandle_ = 0;

	//モデルハンドル
	ModelData modelData_ = {};

	//デフォルトはα加算
	int32_t blendModeNumber_ = 1;


	bool isAnimation_ = false;




};