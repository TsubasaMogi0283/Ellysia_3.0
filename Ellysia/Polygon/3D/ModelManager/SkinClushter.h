#pragma once
#include <vector>

#include "DirectXSetup.h"
#include "Matrix4x4.h"
#include <VertexInfluence.h>
#include <span>
#include <WellForGPU.h>
#include <Skeleton.h>
#include <ModelData.h>
#include <WorldTransform.h>
#include <Camera.h>


struct SkinClusterStruct {
	std::vector<Matrix4x4> inverseBindPoseMatrices;
	
	//Influence
	//頂点に対して影響を与えるパラメータ群
	ComPtr<ID3D12Resource> influenceResource;
	D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
	std::span<VertexInfluence>mappedInfluence;

	//MatrixPalette
	//Skinningを行う際に必要な行列をSkeletonの全Jointの数だけ格納した配列
	ComPtr<ID3D12Resource>paletteResource;
	std::span<WellForGPU> mappedPalette;
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> paletteSrvHandle;
	uint32_t srvIndex;
}; 



class SkinCluster {
public:
	SkinCluster() {};

	//SkinClusterを作る
	void CreateSkinClusher(const Skeleton& skeleton, const ModelData& modelData);

	//SkinClusterの更新
	void Update();

	void Draw(WorldTransform& worldTransform,Camera& camera);

	~SkinCluster() {};



public:
	SkinClusterStruct GetSkinClusterStruct() {
		return skinClusterStruct_;
	}

private:

	SkinClusterStruct skinClusterStruct_ = {};
	Skeleton skeleton_;
	


};


