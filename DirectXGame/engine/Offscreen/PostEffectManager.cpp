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
	DXGIDevice_ = DXGIDevice;
	command_ = command;
	srvManager_ = srvManager;
	rtvManager_ = rtvManager;
	renderingCommon_ = renderingCommon;
	depthStencil_ = depthStencil;
	barrier_ = barrier;
	scissorRect_ = scissorRect;
	viewPort_ = viewPort;




}

void PostEffectManager::PreDrawOffscreen()
{
	// レンダーターゲット
	barrier_->TransitionResource(renderTextures_[0]->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	// デプスステンシル
	barrier_->TransitionResource(depthStencil_->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);



	//// 描画先の設定
	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencil_->GetCPUHandleDepthStencilResorce();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTextures_[0]->GetRTVHandle();
	command_->GetList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);


	//// レンダーターゲットと深度バッファをクリア
	float clearColor[] = { renderTextures_[0]->GetClearColor().x,  renderTextures_[0]->GetClearColor().y, renderTextures_[0]->GetClearColor().z,  renderTextures_[0]->GetClearColor().w }; // 任意のクリアカラー（赤）
	command_->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	command_->GetList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//
	viewPort_->SettingViewport();
	scissorRect_->SettingScissorRect();
}

void PostEffectManager::PostDrawOffscreen()
{
	// レンダーターゲット
	barrier_->TransitionResource(renderTextures_[0]->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// デプスステンシル
	barrier_->TransitionResource(depthStencil_->GetResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void PostEffectManager::AllPostEffect()
{
	if (renderTextures_.size() == 0) return;

	for (int i = 0; i < static_cast<int>(renderTextures_.size() - 1); i++) {
		DrawRenderTexture(renderTextures_[i+1].get(), renderTextures_[i].get()); 
	}
}

void PostEffectManager::Update(Camera* camera)
{
	for (auto& renderTexture : renderTextures_) {
		renderTexture->SetCamera(camera);
	}

#ifdef _DEBUG
	// レンダーテクスチャ
	ImGui::Begin("engine");
	if (ImGui::CollapsingHeader("RenderTexture")) {
		for (auto& renderTexture : renderTextures_) {
			renderTexture->SetCamera(camera);
			renderTexture->Update();
		}
	}
	ImGui::End();
#endif // _DEBUG
}

void PostEffectManager::AddRenderTexture(const std::string name)
{
	auto renderTexture = std::make_unique<RenderTexture>();
	renderTexture->Initialize(DXGIDevice_, command_, srvManager_, rtvManager_, renderingCommon_, name);
	RenderTexture* current = renderTexture.get();
	
	renderTextures_.push_back(std::move(renderTexture));

	endRenderTexture = current;
	
}

void PostEffectManager::DrawRenderTexture(RenderTexture* targetRT, RenderTexture* sourceRT)
{
	
	PreDraw(targetRT);

	// レンダーテクスチャ(コピー)
	sourceRT->Draw();


	PostDraw(targetRT);
}

void PostEffectManager::PreDraw(RenderTexture* renderTexture)
{
	// レンダーターゲット
	barrier_->TransitionResource(renderTexture->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);


	//// 描画先の設定
	// 描画先のRTVとDSVを設定する
	// このポストエフェクトでは深度バッファを使用しないため、DSVは設定しない
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTexture->GetRTVHandle();
	command_->GetList()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);


	//// レンダーターゲットと深度バッファをクリア
	float clearColor[] = { renderTexture->GetClearColor().x,  renderTexture->GetClearColor().y, renderTexture->GetClearColor().z,  renderTexture->GetClearColor().w }; // 任意のクリアカラー（赤）
	command_->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	//
	viewPort_->SettingViewport();
	scissorRect_->SettingScissorRect();
}

void PostEffectManager::PostDraw(RenderTexture* renderTexture)
{
	// レンダーターゲット
	barrier_->TransitionResource(renderTexture->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

