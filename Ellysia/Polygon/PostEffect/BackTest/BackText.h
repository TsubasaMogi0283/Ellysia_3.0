#pragma once
#include "DirectXSetup.h"
#include "Vector4.h"
#include "Vector3.h"
#include "VertexData.h"

enum TextureEffectType {
	NoneEffect,	//エフェクトなし
	Monochrome,	//白黒(グレースケール)
	Sepia,		//セピア
	VignetteEffect,	//端が暗くなる
	
	//Smoothing(平滑化)
	//輪郭などのくっきりしたところをぼかして滑らかな雰囲気を出すよ
	BoxFilter3x3a,	
	BoxFilter5x5a,

	//GaussianFilter
	//BoxFilterよりこっちの方良い感じらしい
	GaussianFilter3x3a,
	GaussianFilter5x5a,



};

struct TextureEffectSelection {
	//白黒(グレースケール)
	bool isMonochrome;
	//セピア
	bool isSepia;
	//端が暗くなる
	bool isVegnette;
		

	//Smoothing(平滑化)
	//輪郭などのくっきりしたところをぼかして滑らかな雰囲気を出すよ
	bool isBoxFilter3x3;
	bool isBoxFilter5x5;

	//GaussianFilter
	//BoxFilterよりこっちの方良い感じらしい
	bool isGaussianFilter3x3;
	bool isGaussianFilter5x5;

};


struct VignetteInformation {
	//倍
	float scale;
	//乗
	float pow;
	
};

struct GaussianFilterInformation {
	//標準偏差
	//StandardDivision。一般的にσ(シグマ)が使われる
	float sigma;

};


class BackText{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	BackText()=default;


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~BackText()=default;

private:
	//別々で分けたい
	ComPtr<ID3D12Resource> effectResource_ = nullptr;

	int32_t effectType_ = NoneEffect;
	int32_t* effectTypeData_ = nullptr;

	//Vignette
	ComPtr<ID3D12Resource> vignetteResource_ = nullptr;
	VignetteInformation vignetteInformation_ = {};
	VignetteInformation* vignetteData_ = nullptr;

	//GaussianFilter
	ComPtr<ID3D12Resource> gaussianFilterResource_ = nullptr;
	GaussianFilterInformation gaussianFilterInformation_ = {};
	GaussianFilterInformation* gaussianFilterData_ = nullptr;


	uint32_t rtvHandle_=0;
	//RTV
	ComPtr<ID3D12Resource> rtvResource_ = nullptr;
	Vector4 renderTargetClearValue_ = {};
	uint32_t srvHandle_ = 0;

	D3D12_RESOURCE_BARRIER barrier = {};
};