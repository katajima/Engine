#include "PostEffectManager.h"

#include "DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/Manager/RTV/RtvManager.h"
#include "DirectXGame/engine/Offscreen/RenderingCommon.h"
#include "DirectXGame/engine/DirectX/DepthStencil/DepthStencil.h"
#include "DirectXGame/engine/DirectX/Barrier/Barrier.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/ScissorRect/ScissorRect.h"
#include "DirectXGame/engine/DirectX/ViewPort/ViewPort.h"
#include "DirectXGame/engine/scene/SceneManager.h"

#include"imgui.h"


void Engine::PostEffectManager::Initialize(DXGIDevice* DXGIDevice, Command* command, SrvManager* srvManager, RtvManager* rtvManager, RenderingCommon* renderingCommon, DepthStencil* depthStencil, Barrier* barrier, ScissorRect* scissorRect, ViewPort* viewPort)
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
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencil->GetCPUHandleDepthStencilResource();
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
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencil->GetCPUHandleDepthStencilResource();
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
	// ゲーム画面を表示する専用ウィンドウを開始する。
	ImGui::Begin("GameScene");

	// 他のドッキングUIを除いた、現在のウィンドウ内で使用できる領域を取得する。
	const ImVec2 availableSize = ImGui::GetContentRegionAvail();
	// レンダーテクスチャの基準となるクライアント領域の幅を取得する。
	const float clientWidth = static_cast<float>(WinApp::GetClientWidth());
	// レンダーテクスチャの基準となるクライアント領域の高さを取得する。
	const float clientHeight = static_cast<float>(WinApp::GetClientHeight());
	// 画面サイズが一時的に0になった場合でも、ゼロ除算を避けるための安全な比率を設定する。
	const float sourceAspect = clientHeight > 0.0f ? clientWidth / clientHeight : 1.0f;
	// 使用可能領域いっぱいに合わせた場合の画像幅を初期値にする。
	float imageWidth = availableSize.x;
	// 元の画面比率を維持した画像高さを計算する。
	float imageHeight = sourceAspect > 0.0f ? imageWidth / sourceAspect : availableSize.y;
	// 高さが領域を超える場合は、高さを基準に画像サイズを再計算する。
	if (imageHeight > availableSize.y) {
		imageHeight = availableSize.y;
		imageWidth = imageHeight * sourceAspect;
	}
	// ウィンドウが極端に小さい場合に負のサイズをImGuiへ渡さないようにする。
	imageWidth = imageWidth > 0.0f ? imageWidth : 0.0f;
	imageHeight = imageHeight > 0.0f ? imageHeight : 0.0f;

	// 余った横方向の領域を分けて、ゲーム画面を中央に配置する。
	const float horizontalPadding = (availableSize.x - imageWidth) * 0.5f;
	if (horizontalPadding > 0.0f) {
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + horizontalPadding);
	}
	// 描画前のカーソル位置を保存し、UI座標変換に利用する。
	const ImVec2 imagePos = ImGui::GetCursorScreenPos();
	// 最終レンダーテクスチャを、計算した利用可能サイズで表示する。
	const ImTextureID imguiTexture = (ImTextureID)(GetEndRenderTexture()->GetSRVGPUHandle().ptr);
	ImGui::Image(imguiTexture, ImVec2(imageWidth, imageHeight));

	// ゲーム画面の左上座標・サイズ・基準画面に対する比率を更新する。
	imageleftTopPos_ = Vector2(imagePos.x, imagePos.y);
	imageSize_ = Vector2(imageWidth, imageHeight);
	imageRatio_ = Vector2{
		clientWidth > 0.0f ? imageWidth / clientWidth : 0.0f,
		clientHeight > 0.0f ? imageHeight / clientHeight : 0.0f
	};

	// ゲーム画面を表示する専用ウィンドウを終了する。
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
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencil->GetCPUHandleDepthStencilResource();
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
