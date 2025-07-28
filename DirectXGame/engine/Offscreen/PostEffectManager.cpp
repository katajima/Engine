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
#include "DirectXGame/engine/scene/SceneManager.h"

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
	renderTexture_->Initialize(DXGIDevice_, command_, srvManager_, rtvManager_, renderingCommon_, "mainStr", PostEffectType::kCopy);
	
	renderTextureEnd_ = std::make_unique<RenderTexture>();
	renderTextureEnd_->Initialize(DXGIDevice_, command_, srvManager_, rtvManager_, renderingCommon_, "mainEnd", PostEffectType::kCopy);
	

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

void PostEffectManager::PreDraw2dOffscreen()
{
	// レンダーターゲット
	barrier_->TransitionResource(renderTextureEnd_->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	// デプスステンシル
	barrier_->TransitionResource(depthStencil_->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);



	//// 描画先の設定
	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencil_->GetCPUHandleDepthStencilResorce();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTextureEnd_->GetRTVHandle();
	command_->GetList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);


	//// レンダーターゲットと深度バッファをクリア
	float clearColor[] = { renderTextureEnd_->GetClearColor().x,  renderTextureEnd_->GetClearColor().y, renderTextureEnd_->GetClearColor().z,  renderTextureEnd_->GetClearColor().w }; // 任意のクリアカラー（赤）
	command_->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	command_->GetList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//
	viewPort_->SettingViewport();
	scissorRect_->SettingScissorRect();
}

void PostEffectManager::PostDraw2dOffscreen()
{
	// レンダーターゲット
	barrier_->TransitionResource(renderTextureEnd_->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// デプスステンシル
	barrier_->TransitionResource(depthStencil_->GetResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}


void PostEffectManager::AllPostEffect(SceneManager* sceneManager)
{

	RenderTexture* previousTexture = renderTexture_.get();
	

	std::sort(effectBlocks_.begin(), effectBlocks_.end(),
		[](const PostEffectBlock* a, const PostEffectBlock* b) {
			return a->GetIndex() < b->GetIndex();
		});

	// インデックスを再設定（連番にする）
	for (size_t i = 0; i < effectBlocks_.size(); ++i) {
		effectBlocks_[i]->SetIndex(static_cast<int>(i));
	}


	for (size_t i = 0; i < effectBlocks_.size(); ++i) {
		auto& current = effectBlocks_[i];
		// 無効なブロックはスキップ
		if (!current->GetUse()) {
			continue;
		}
		// 有効なブロックに描画させる
		current->DrawEffectBlock(previousTexture);

		// 次のブロックの入力用に更新
		previousTexture = current->GetEndRenderTexture();
	}

	PreDraw2dOffscreen();
	previousTexture->Draw();
	sceneManager->Draw2D();
	PostDraw2dOffscreen();
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

void PostEffectManager::AddEffectBlock(const std::string name, PostEffectBlockType type, bool use)
{
	////auto effectBlock = std::make_unique<PostEffectBlock>();
	////effectBlock->Intialize(DXGIDevice_, command_, srvManager_, rtvManager_, renderingCommon_, depthStencil_, barrier_, scissorRect_, viewPort_,name,type);
	////effectBlock->SetUse(use);			// 使うか
	////effectBlock->SetIndex(indexCount_); // 順番
	//effectBlocks_.push_back(effectBlock);
	//indexCount_++; // 加算
}

void PostEffectManager::AddEffectBlocks(std::vector<PostEffectBlock*> effectBlocks)
{
	for (auto& effect  : effectBlocks) {
		effectBlocks_.push_back(effect);
	}

	//effectBlocks_.push_back(std::move(effectBlock));
}

void PostEffectManager::ClearPostEffectBlock()
{
	effectBlocks_.clear();
}

void PostEffectManager::PreEnd(RenderTexture* renderTexture)
{
	// レンダーターゲット
	barrier_->TransitionResource(renderTexture->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);


	//// 描画先の設定
	// 描画先のRTVとDSVを設定する
	// このポストエフェクトでは深度バッファを使用しないため、DSVは設定しない
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTexture->GetRTVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencil_->GetCPUHandleDepthStencilResorce();
	command_->GetList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);


	//// レンダーターゲットと深度バッファをクリア
	float clearColor[] = { renderTexture->GetClearColor().x,  renderTexture->GetClearColor().y, renderTexture->GetClearColor().z,  renderTexture->GetClearColor().w }; // 任意のクリアカラー（赤）
	command_->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	//
	viewPort_->SettingViewport();
	scissorRect_->SettingScissorRect();
}

void PostEffectManager::PostEnd(RenderTexture* renderTexture)
{
	// レンダーターゲット
	barrier_->TransitionResource(renderTexture->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}
