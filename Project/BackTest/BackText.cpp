#include "BackText.h"
#include <PipelineManager.h>
#include "TextureManager.h"
#include <SrvManager.h>

void BackText::Initialize(){
	PipelineManager::GetInstance()->GenarateCopyImagePSO();
	
	//ここでBufferResourceを作る
	vertexResouce_ = DirectXSetup::GetInstance()->CreateBufferResource(sizeof(VertexData) * 3);
	

	centerResource_ = DirectXSetup::GetInstance()->CreateBufferResource(sizeof(Vector3));

	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResouce_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点３つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 3;
	//１頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	//書き込むためのアドレスを取得
	vertexResouce_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	materialResource_ = DirectXSetup::GetInstance()->CreateBufferResource(sizeof(Vector4));
	
	textureHandle_ = SrvManager::GetInstance()->Allocate();
	SrvManager::GetInstance()->CreateSRVForRenderTexture(DirectXSetup::GetInstance()->GetRenderTextureResource().Get(), textureHandle_);
}

void BackText::PreDraw(){
	//ResourceとHandleはDirectX側で作った
	//いずれRTV・DSVManagerを作る
	
	// Barrierを設定する
	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = DirectXSetup::GetInstance()->GetRenderTextureResource().Get();
	// 遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	// 遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// TransitionBarrierを張る
	DirectXSetup::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);

	
	const float RENDER_TARGET_CLEAR_VALUE[] = { 1.0f,0.0f,0.0f,1.0f };
	DirectXSetup::GetInstance()->GetCommandList()->OMSetRenderTargets(
		1, &DirectXSetup::GetInstance()->GetRtvHandle(2), false, &DirectXSetup::GetInstance()->GetDsvHandle());

	DirectXSetup::GetInstance()->GetCommandList()->ClearRenderTargetView(
		DirectXSetup::GetInstance()->GetRtvHandle(2), RENDER_TARGET_CLEAR_VALUE, 0, nullptr);
	DirectXSetup::GetInstance()->GetCommandList()->ClearDepthStencilView(
		DirectXSetup::GetInstance()->GetDsvHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = float(WindowsSetup::GetInstance()->GetClientWidth());
	viewport.Height = float(WindowsSetup::GetInstance()->GetClientHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	DirectXSetup::GetInstance()->GetCommandList()->RSSetViewports(1, &viewport);


	//シザー矩形 
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = WindowsSetup::GetInstance()->GetClientWidth();
	scissorRect.top = 0;
	scissorRect.bottom = WindowsSetup::GetInstance()->GetClientHeight();

	DirectXSetup::GetInstance()->GetCommandList()->RSSetScissorRects(1, &scissorRect);




}

void BackText::Draw(){
	
	//左上
	vertexData_[0].position = { 0.0f,0.0f,0.0f,1.0f };
	vertexData_[0].texCoord = { 0.0f,1.0f };
	//右上
	vertexData_[1].position = { 0.2f,0.0f,0.0f,1.0f };
	vertexData_[1].texCoord = { 0.5f,0.0f };
	//左下
	vertexData_[2].position = { 0.0f,-0.5f,0.0f,1.0f };
	vertexData_[2].texCoord = { 1.0f,1.0f };
	//範囲外は危険だよ！！

	centerResource_->Map(0, nullptr, reinterpret_cast<void**>(&positiondata_));
	*positiondata_ = centerPosition_;

	//マテリアルにデータを書き込む
	//書き込むためのアドレスを取得
	//reinterpret_cast...char* から int* へ、One_class* から Unrelated_class* へなどの変換に使用
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	//今回は赤を書き込んでみる
	*materialData_ = color_;

	DirectXSetup::GetInstance()->GetCommandList()->SetGraphicsRootSignature(PipelineManager::GetInstance()->GetCopyImageRootSignature().Get());
	DirectXSetup::GetInstance()->GetCommandList()->SetPipelineState(PipelineManager::GetInstance()->GetCopyImageGraphicsPipelineState().Get());

	//RootSignatureを設定。PSOに設定しているけど別途設定が必要
	DirectXSetup::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	//形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えよう
	DirectXSetup::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//マテリアルCBufferの場所を設定
	//ここでの[0]はregisterの0ではないよ。rootParameter配列の0番目
	DirectXSetup::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	//Position
	DirectXSetup::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, centerResource_->GetGPUVirtualAddress());

	//Texture
	TextureManager::GraphicsCommand(textureHandle_);

	//描画(DrawCall)３頂点で１つのインスタンス。
	DirectXSetup::GetInstance()->GetCommandList()->DrawInstanced(3, 1, 0, 0);

	
}

void BackText::PostDraw(){
	
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = DirectXSetup::GetInstance()->GetRenderTextureResource().Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	DirectXSetup::GetInstance()->GetCommandList()->ResourceBarrier(1, &barrier);
}