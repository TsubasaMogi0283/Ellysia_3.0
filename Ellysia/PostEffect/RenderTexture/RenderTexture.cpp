#include "RenderTexture.h"
#include <PipelineManager.h>

RenderTexture::RenderTexture(){

}

void RenderTexture::Initialize(){

}

void RenderTexture::BeginDraw(){
	//vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	//vertexData_->position = { left,bottom,0.0f,1.0f };
	//vertexData_[LEFT_BOTTOM].texCoord = { texLeft,texBottom };

	////左上
	//vertexData_[LEFT_TOP].position = { left,top,0.0f,1.0f };
	//vertexData_[LEFT_TOP].texCoord = { texLeft,texTop };

	////右下
	//vertexData_[RIGHT_BOTTOM].position = { right,bottom,0.0f,1.0f };
	//vertexData_[RIGHT_BOTTOM].texCoord = { texRight,texBottom };


	////右上
	//vertexData_[RIGHT_TOP].position = { right,top,0.0f,1.0f };
	//vertexData_[RIGHT_TOP].texCoord = { texRight,texTop };


	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;


	backBufferIndex_ = DirectXSetup::GetInstance()->GetSwapChain().m_pSwapChain->GetCurrentBackBufferIndex();

	////TransitionBarrierを張るコード
	//現在のResourceStateを設定する必要がある → ResorceがどんなStateなのかを追跡する必要がある
	//追跡する仕組みはStateTrackingという
	//
	//TransitionBarrierの設定
	//今回のバリアはTransition
	barrier_.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにする
	barrier_.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier_.Transition.pResource = DirectXSetup::GetInstance()->GetSwapChain().m_pResource[backBufferIndex_].Get();
	//遷移前(現在)のResourceState
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//遷移後のResourceState
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//TransitionBarrierを張る
	DirectXSetup::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier_);


	DirectXSetup::GetInstance()->GetCommandList()->SetGraphicsRootSignature(PipelineManager::GetInstance()->GetCopyImageRootSignature().Get());
	DirectXSetup::GetInstance()->GetCommandList()->SetPipelineState(PipelineManager::GetInstance()->GetCopyImageGraphicsPipelineState().Get());



	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	//DirectXSetup::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	DirectXSetup::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);




	//描画(DrawCall)３頂点で１つのインスタンス。
	DirectXSetup::GetInstance()->GetCommandList()->DrawInstanced(3, 1, 0, 0);

}

void RenderTexture::EndDraw(){
	barrier_.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	barrier_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを張る
	DirectXSetup::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier_);

}

RenderTexture::~RenderTexture(){

}