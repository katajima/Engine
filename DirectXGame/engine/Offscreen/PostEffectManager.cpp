#include "PostEffectManager.h"

#include "DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/Manager/RTV/RtvManeger.h"
#include "DirectXGame/engine/Offscreen/RenderingCommon.h"
#include "DirectXGame/engine/DirectX/DepthStencil/DepthStencil.h"
#include "DirectXGame/engine/DirectX/Barrier/Barrier.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/ScissorRect/ScissorRect.h"
#include "DirectXGame/engine/DirectX/ViewPort/ViewPort.h"


#include"imgui.h"

void PostEffectManager::Intialize(DXGIDevice* DXGIDevice, Command* command, SrvManager* srvManager, RtvManager* rtvManager, RenderingCommon* renderingCommon, DepthStencil* depthStencil, Barrier* barrier, ScissorRect* scissorRect, ViewPort* viewPort)
{
	DXGIDevice_		= DXGIDevice;
	command_		= command;
	srvManager_		= srvManager;
	rtvManager_		= rtvManager;
	renderingCommon_ = renderingCommon;
	depthStencil_	= depthStencil;
	barrier_		= barrier;
	scissorRect_	= scissorRect;
	viewPort_		= viewPort;


	renderTexture_ = std::make_unique<RenderTexture>();
	renderTexture_->Initialize(DXGIDevice_, command_, srvManager_, rtvManager_, renderingCommon_, "main");
	renderTexture_->type_ = RenderTexture::PostEffectType::kCopy;
}

void PostEffectManager::PreDrawOffscreen()
{
	// レンダーターゲット
	barrier_->TransitionResource(renderTexture_->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	// デプスステンシル
	barrier_->TransitionResource(depthStencil_->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);



	//// 描画先の設定
	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencil_->GetCPUHandleDepthStencilResorce();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTexture_->GetRTVHandle();
	command_->GetList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);


	//// レンダーターゲットと深度バッファをクリア
	float clearColor[] = { renderTexture_->GetClearColor().x,  renderTexture_->GetClearColor().y, renderTexture_->GetClearColor().z,  renderTexture_->GetClearColor().w }; // 任意のクリアカラー（赤）
	command_->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	command_->GetList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//
	viewPort_->SettingViewport();
	scissorRect_->SettingScissorRect();
}

void PostEffectManager::PostDrawOffscreen()
{
	// レンダーターゲット
	barrier_->TransitionResource(renderTexture_->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// デプスステンシル
	barrier_->TransitionResource(depthStencil_->GetResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void PostEffectManager::AllPostEffect()
{

	if (effectBlocks_.size() == 0) return;

	for (int i = 0; i < static_cast<int>(effectBlocks_.size()); i++) {

		if (i == 0) {
			// 最初なので作っていたRenderTextureを渡す
			effectBlocks_[i]->ConnectBlock(renderTexture_.get());	
		}
		else {
			effectBlocks_[i]->ConnectBlock(effectBlocks_[i-1]->GetEndRenderTexture());
		}

		effectBlocks_[i]->DrawEffectBlock();
	}
}

void PostEffectManager::Update(Camera* camera)
{
	renderingCommon_->SetCamera(camera);
	renderTexture_->SetCamera(camera);
	renderTexture_->Update();

	for (auto& effect : effectBlocks_) {
		effect->Update(camera);
	}
}

void PostEffectManager::AddEffectBlock(const std::string name,PostEffectType type)
{
	auto effectBlock = std::make_unique<PostEffectBlock>();
	effectBlock->Intialize(DXGIDevice_, command_, srvManager_, rtvManager_, renderingCommon_, depthStencil_, barrier_, scissorRect_, viewPort_,name,type);
	effectBlocks_.push_back(std::move(effectBlock));
}
