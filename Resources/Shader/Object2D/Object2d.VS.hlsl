#include "Object2d.hlsli"

//座標返還を行うVS
struct TransformationMatrix {
	//32bitのfloatが4x4個
	float4x4 WVP;
	float4x4 World;
};

//CBuffer
ConstantBuffer<TransformationMatrix> gTransformationMatrix:register(b0);

struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};


VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	//mul...組み込み関数
	output.position = mul(input.position,gTransformationMatrix.WVP);
	output.texcoord = input.texcoord;
	//法線の変換にはWorldMatrixの平衡移動は不要。拡縮回転情報が必要
	//左上3x3だけを取り出す
	//法線と言えば正規化をなのでそれを忘れないようにする
	output.normal = normalize(mul(input.normal, (float3x3)gTransformationMatrix.World));
	return output;
}