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


void Engine::PostEffectManager::Intialize(DXGIDevice* DXGIDevice, Command* command, SrvManager* srvManager, RtvManager* rtvManager, RenderingCommon* renderingCommon, DepthStencil* depthStencil, Barrier* barrier, ScissorRect* scissorRect, ViewPort* viewPort)
{
	this->dxgiDevice = DXGIDevice;				// デバイス
	this->command = command;						// コマンド
	this->srvManager = srvManager;				// SRV管理
	this->rtvManager = rtvManager;				// RTV管理
	this->renderingCommon = renderingCommon;		// レンダリング共通クラス
	this->depthStencil = depthStencil;			// デプスステンシル
	this->barrier = barrier;						// バリア
	this->scissorRect = scissorRect;				// シザー
	this->viewPort = viewPort;					// ビューポート

	// レンダーテクスチャ初期化
	renderTexture_ = std::make_unique<RenderTexture>();
	renderTexture_->Initialize(dxgiDevice, command, srvManager, rtvManager, renderingCommon, "mainStr", PostEffectType::kCopy);

	// 最終レンダーテクスチャテクスチャ初期化
	renderTextureEnd_ = std::make_unique<RenderTexture>();
	renderTextureEnd_->Initialize(dxgiDevice, command, srvManager, rtvManager, renderingCommon, "mainEnd", PostEffectType::kCopy);


}

void Engine::PostEffectManager::PreDrawOffscreen()
{
	// レンダーターゲット
	barrier->TransitionResource(renderTexture_->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	// デプスステンシル
	barrier->TransitionResource(depthStencil->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);



	//// 描画先の設定
	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencil->GetCPUHandleDepthStencilResorce();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTexture_->GetRTVHandle();
	command->GetList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);


	//// レンダーターゲットと深度バッファをクリア
	float clearColor[] = { renderTexture_->GetClearColor().x,  renderTexture_->GetClearColor().y, renderTexture_->GetClearColor().z,  renderTexture_->GetClearColor().w }; // 任意のクリアカラー（赤）
	command->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	command->GetList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//
	viewPort->SettingViewport();
	scissorRect->SettingScissorRect();
}

void Engine::PostEffectManager::PostDrawOffscreen()
{
	// レンダーターゲット
	barrier->TransitionResource(renderTexture_->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// デプスステンシル
	barrier->TransitionResource(depthStencil->GetResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}



void Engine::PostEffectManager::PreDraw2dOffscreen()
{
	// レンダーターゲット
	barrier->TransitionResource(renderTextureEnd_->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	// デプスステンシル
	barrier->TransitionResource(depthStencil->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);



	//// 描画先の設定
	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencil->GetCPUHandleDepthStencilResorce();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTextureEnd_->GetRTVHandle();
	command->GetList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);


	//// レンダーターゲットと深度バッファをクリア
	float clearColor[] = { renderTextureEnd_->GetClearColor().x,  renderTextureEnd_->GetClearColor().y, renderTextureEnd_->GetClearColor().z,  renderTextureEnd_->GetClearColor().w }; // 任意のクリアカラー（赤）
	command->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	command->GetList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//
	viewPort->SettingViewport();
	scissorRect->SettingScissorRect();
}

void Engine::PostEffectManager::PostDraw2dOffscreen()
{
	// レンダーターゲット
	barrier->TransitionResource(renderTextureEnd_->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// デプスステンシル
	barrier->TransitionResource(depthStencil->GetResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}


void Engine::PostEffectManager::AllPostEffect(SceneManager* sceneManager)
{
	RenderTexture* previousTexture = renderTexture_.get();

	for (PostEffectPipeline* pipeline : pipelines_) {
		if (!pipeline) {
			continue;
		}
		RenderTexture* result = pipeline->Execute(previousTexture);
		if (result) {
			previousTexture = result;
		}
	}

	PreDraw2dOffscreen();
	if (previousTexture) {
		previousTexture->Draw();
	}
	PostDraw2dOffscreen();
}

void Engine::PostEffectManager::Update(Camera* camera)
{
	imageRatio_ = { 1.0f,1.0f };
	imageleftTopPos_ = { 0.0f,0.0f };
	renderingCommon->SetCamera(camera);
	renderTexture_->SetCamera(camera);
	renderTexture_->Update();
	for (auto& pipeline : pipelines_) {
		if (pipeline) {
			pipeline->Update(camera);
		}
	}
}



void Engine::PostEffectManager::AddPipeline(PostEffectPipeline* pipeline)
{
	if (pipeline) {
		pipelines_.push_back(pipeline);
	}
}

void Engine::PostEffectManager::ClearPostEffectBlock()
{
	pipelines_.clear();
}

void Engine::PostEffectManager::RenderImGui()
{
	ImGui::Begin("GameScene");
	float width = static_cast<float> (WinApp::GetClientWidth() / 1.5f);
	float height = static_cast<float> (WinApp::GetClientHeight() / 1.5f);

	ImTextureID imguiTexture = (ImTextureID)(GetEndRenderTexture()->GetSRVGPUHandle().ptr);
	ImGui::Image(imguiTexture, ImVec2(width, height));
	ImVec2 imagePos = ImGui::GetCursorScreenPos();
	imagePos.y -= height;

	Vector2 windowSize = Vector2{ static_cast<float> (WinApp::GetClientWidth()),static_cast<float> (WinApp::GetClientHeight()) };
	Vector2 imageSize = Vector2{ width,height };

	ImGui::InputFloat2("GetCursorScreenPos", &imagePos.x);
	ImGui::InputFloat2("WindowSize", &windowSize.x);
	ImGui::InputFloat2("ImageSize", &imageSize.x);


	imageleftTopPos_ = Vector2(imagePos.x, imagePos.y);
	imageSize_ = Vector2(width, height);
	imageRatio_ = Vector2{ width / windowSize.x,height / windowSize.y };
	ImGui::InputFloat2("ImageleftTopPos", &imageleftTopPos_.x);
	ImGui::InputFloat2("ImageRatio", &imageRatio_.x);

	ImGui::End();
}

void Engine::PostEffectManager::RenderUpdate()
{
	float width = static_cast<float> (WinApp::GetClientWidth());
	float height = static_cast<float> (WinApp::GetClientHeight());
}

void Engine::PostEffectManager::PreEnd(RenderTexture* renderTexture)
{
	// レンダーターゲット
	barrier->TransitionResource(renderTexture->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);


	//// 描画先の設定
	// 描画先のRTVとDSVを設定する
	// このポストエフェクトでは深度バッファを使用しないため、DSVは設定しない
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTexture->GetRTVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencil->GetCPUHandleDepthStencilResorce();
	command->GetList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);


	//// レンダーターゲットと深度バッファをクリア
	float clearColor[] = { renderTexture->GetClearColor().x,  renderTexture->GetClearColor().y, renderTexture->GetClearColor().z,  renderTexture->GetClearColor().w }; // 任意のクリアカラー（赤）
	command->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	//
	viewPort->SettingViewport();
	scissorRect->SettingScissorRect();
}

void Engine::PostEffectManager::PostEnd(RenderTexture* renderTexture)
{
	// レンダーターゲット
	barrier->TransitionResource(renderTexture->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}
