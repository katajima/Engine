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
void PostEffectBlock::Intialize(DXGIDevice* DXGIDevice, Command* command, SrvManager* srvManager, RtvManager* rtvManager, RenderingCommon* renderingCommon, 
	DepthStencil* depthStencil, Barrier* barrier, ScissorRect* scissorRect, ViewPort* viewPort, 
	const std::string name, PostEffectType type)
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

	name_ = name;
	switch (type)
	{
	case PostEffectType::kCopy:
		AddRenderTexture("Copy_" + name, RenderTexture::PostEffectType::kCopy);
		break;
	case PostEffectType::kGrayScale:
		AddRenderTexture("GrayScale_" + name, RenderTexture::PostEffectType::kGrayScale);
		break;
	case PostEffectType::kSepia:
		AddRenderTexture("Sepia_" + name, RenderTexture::PostEffectType::kSepia);
		break;
	case PostEffectType::kVignette:
		AddRenderTexture("Vignette_" + name, RenderTexture::PostEffectType::kVignette);
		break;
	case PostEffectType::kSmoothing:
		AddRenderTexture("Smoothing_" + name, RenderTexture::PostEffectType::kSmoothing);
		break;
	case PostEffectType::kGaussian:
		AddRenderTexture("Gaussian_" + name, RenderTexture::PostEffectType::kGaussian);
		break;
	case PostEffectType::kOitline:
		AddRenderTexture("Oitline_" + name, RenderTexture::PostEffectType::kOitline);
		break;
	case PostEffectType::kRadialBlur:
		AddRenderTexture("RadialBlur_" + name, RenderTexture::PostEffectType::kRadialBlur);
		break;
	case PostEffectType::kDissovle:
		AddRenderTexture("Dissovle_" + name, RenderTexture::PostEffectType::kDissovle);
		break;
	case PostEffectType::kRandom:
		AddRenderTexture("Random_" + name, RenderTexture::PostEffectType::kRandom);
		break;
	case PostEffectType::kBloom:
		//AddRenderTexture("Bloom_1_" + name, RenderTexture::PostEffectType::kCopy);
		AddRenderTexture("Bloom_1_" + name, RenderTexture::PostEffectType::kBloom);
		AddRenderTexture("Bloom_2_" + name, RenderTexture::PostEffectType::kGaussian);
		AddRenderTexture("Bloom_3_" + name, RenderTexture::PostEffectType::kBloomCombin);
		GetRenderTextures(2)->SetOtherSrvIndex(GetRenderTextures(0)->GetSrvIndex());

		break;
	}


}

// 更新
void PostEffectBlock::Update(Camera* camera)
{
	for (auto& renderTexture : renderTextures_) {
		renderTexture->SetCamera(camera);
		renderTexture->Update();
	}

#ifdef _DEBUG
	// レンダーテクスチャ
	ImGui::Begin("engine");
	if (ImGui::CollapsingHeader(name_.c_str())) {
		for (auto& renderTexture : renderTextures_) {
			renderTexture->Update();
		}
	}
	ImGui::End();
#endif // _DEBUG
}

// 追加
void PostEffectBlock::AddRenderTexture(const std::string name, RenderTexture::PostEffectType type)
{
	auto renderTexture = std::make_unique<RenderTexture>();
	renderTexture->Initialize(DXGIDevice_, command_, srvManager_, rtvManager_, renderingCommon_, name);
	renderTexture->type_ = type;
	
	renderTextures_.push_back(std::move(renderTexture));
}

void PostEffectBlock::DrawRenderTexture(RenderTexture* targetRT, RenderTexture* sourceRT)
{

	PreDraw(targetRT);

	// レンダーテクスチャ(コピー)
	sourceRT->Draw();


	PostDraw(targetRT);
}

void PostEffectBlock::DrawEffectBlock(RenderTexture* inputTexture)
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

void PostEffectBlock::ConnectBlock(RenderTexture* input)
{
	DrawEffectBlock(input);
}


// Pre
void PostEffectBlock::PreDraw(RenderTexture* renderTexture)
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

// Post
void PostEffectBlock::PostDraw(RenderTexture* renderTexture)
{
	// レンダーターゲット
	barrier_->TransitionResource(renderTexture->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}