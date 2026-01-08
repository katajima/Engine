#include "PostEffectBlock.h"

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

// 初期化
void Engine::PostEffectBlock::Intialize(DXGIDevice* DXGIDevice, Command* command, SrvManager* srvManager, RtvManager* rtvManager, RenderingCommon* renderingCommon,
	DepthStencil* depthStencil, Barrier* barrier, ScissorRect* scissorRect, ViewPort* viewPort, 
	const std::string name, PostEffectBlockType type)
{
	DXGIDevice_ = DXGIDevice;				// デバイス
	command_ = command;						// コマンド
	srvManager_ = srvManager;				// SRV管理
	rtvManager_ = rtvManager;				// RTV管理
	renderingCommon_ = renderingCommon;		// レンダリング共通クラス
	depthStencil_ = depthStencil;			// デプスステンシル
	barrier_ = barrier;						// バリア
	scissorRect_ = scissorRect;				// シザー
	viewPort_ = viewPort;					// ビューポート

	name_ = name;	// 名前

	// タイプに応じて各ポストエフェクトを追加する処理
	switch (type)
	{
	case PostEffectBlockType::kCopy:
		AddRenderTexture("Copy_" + name, Engine::PostEffectType::kCopy);
		break;
	case PostEffectBlockType::kGrayScale:
		AddRenderTexture("GrayScale_" + name, ::Engine::PostEffectType::kGrayScale);
		break;
	case PostEffectBlockType::kSepia:
		AddRenderTexture("Sepia_" + name, Engine::PostEffectType::kSepia);
		break;
	case PostEffectBlockType::kVignette:
		AddRenderTexture("Vignette_" + name, Engine::PostEffectType::kVignette);
		break;
	case PostEffectBlockType::kSmoothing:
		AddRenderTexture("Smoothing_" + name, Engine::PostEffectType::kSmoothing);
		break;
	case PostEffectBlockType::kGaussian:
		AddRenderTexture("Gaussian_" + name, Engine::PostEffectType::kGaussian);
		break;
	case PostEffectBlockType::kOitline:
		AddRenderTexture("Oitline_" + name, Engine::PostEffectType::kOitline);
		break;
	case PostEffectBlockType::kRadialBlur:
		AddRenderTexture("RadialBlur_" + name, Engine::PostEffectType::kRadialBlur);
		break;
	case PostEffectBlockType::kDissovle:
		AddRenderTexture("Dissovle_" + name, Engine::PostEffectType::kDissovle);
		break;
	case PostEffectBlockType::kRandom:
		AddRenderTexture("Random_" + name, Engine::PostEffectType::kRandom);
		break;
	case PostEffectBlockType::kBloom:
		AddRenderTexture("BrightPassFilter" + name, Engine::PostEffectType::kBloom);
		AddRenderTexture("Gaussian" + name, Engine::PostEffectType::kGaussian);
		AddRenderTexture("BloomCombine" + name, Engine::PostEffectType::kBloomCombin);
		GetRenderTextures(2)->SetOtherSrvIndex(GetRenderTextures(0)->GetSrvIndex());

		break;
	}


}

// 更新
void Engine::PostEffectBlock::Update(Camera* camera)
{
	if (!renderTextures_.empty()) {
		for (auto& renderTexture : renderTextures_) {
			renderTexture->SetCamera(camera);
			renderTexture->Update();
		}

#ifdef _DEBUG
	// レンダーテクスチャ
	ImGui::Begin("engine");
	if (ImGui::CollapsingHeader(name_.c_str())) {
		ImGui::Checkbox("use",&use_);
		int index = static_cast<int>(index_);
		ImGui::DragInt("index", &index);
		if (index <= 0) {
			index = 0;
		}
		index_ = static_cast<uint32_t>(index);
		for (auto& renderTexture : renderTextures_) {
			renderTexture->Update();
		}
	}
	ImGui::End();
#endif // _DEBUG
	}
}

// 追加
void Engine::PostEffectBlock::AddRenderTexture(const std::string name, PostEffectType type)
{
	auto renderTexture = std::make_unique<RenderTexture>();
	renderTexture->Initialize(DXGIDevice_, command_, srvManager_, rtvManager_, renderingCommon_, name, type);
	
	renderTextures_.push_back(std::move(renderTexture));
}

void Engine::PostEffectBlock::DrawRenderTexture(RenderTexture* targetRT, RenderTexture* sourceRT)
{

	PreDraw(targetRT);

	// レンダーテクスチャ(コピー)
	sourceRT->Draw();


	PostDraw(targetRT);
}

void Engine::PostEffectBlock::DrawEffectBlock(RenderTexture* inputTexture)
{
	if (renderTextures_.empty()) return;

	if (renderTextures_.size() == 1) {
		// 入力テクスチャを最初のレンダーターゲットに書き込む（処理なしコピー）
		DrawRenderTexture(renderTextures_[0].get(), inputTexture);
		return;
	}
	else {
		// 最初の入力
		DrawRenderTexture(renderTextures_[0].get(), inputTexture);

		// 2番目以降でチェーン処理
		for (size_t i = 0; i < renderTextures_.size() - 1; ++i){
			DrawRenderTexture(renderTextures_[i + 1].get(), renderTextures_[i].get());
		}


	}
	
}

void Engine::PostEffectBlock::ConnectBlock(RenderTexture* input)
{
	DrawEffectBlock(input);
}


// Pre
void Engine::PostEffectBlock::PreDraw(RenderTexture* renderTexture)
{
	// レンダーターゲット
	barrier_->TransitionResource(renderTexture->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//barrier_->TransitionResource(depthStencil_->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);

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

// Post
void Engine::PostEffectBlock::PostDraw(RenderTexture* renderTexture)
{
	// レンダーターゲット
	barrier_->TransitionResource(renderTexture->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	//barrier_->TransitionResource(depthStencil_->GetResource(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}