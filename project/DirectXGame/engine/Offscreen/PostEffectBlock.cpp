#include "PostEffectBlock.h"

#include <algorithm>

#include "DirectXGame/engine/Offscreen/PostEffect.h"
#include "DirectXGame/engine/DirectX/Barrier/Barrier.h"
#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/DepthStencil/DepthStencil.h"
#include "DirectXGame/engine/DirectX/ScissorRect/ScissorRect.h"
#include "DirectXGame/engine/DirectX/ViewPort/ViewPort.h"
#include "DirectXGame/engine/Manager/RTV/RtvManager.h"
#include "DirectXGame/engine/Manager/SRV/SrvManager.h"

void Engine::PostEffectPass::Initialize(DXGIDevice* dxgiDevice, Command* command, SrvManager* srvManager,
	RtvManager* rtvManager, RenderingCommon* renderingCommon,
	DepthStencil* depthStencil, Barrier* barrier,
	ScissorRect* scissorRect, ViewPort* viewPort,
	const std::string& name, PostEffectType type)
{
	name_ = name;
	type_ = type;
	command_ = command;
	depthStencil_ = depthStencil;
	barrier_ = barrier;
	scissorRect_ = scissorRect;
	viewPort_ = viewPort;

	renderTexture_ = std::make_unique<RenderTexture>();
	renderTexture_->Initialize(dxgiDevice, command, srvManager, rtvManager, renderingCommon, name, type);
}

void Engine::PostEffectPass::Update(Camera* camera)
{
	if (!renderTexture_) {
		return;
	}

	renderTexture_->SetCamera(camera);
	renderTexture_->Update();
}

void Engine::PostEffectPass::Draw(RenderTexture* inputTexture)
{
	if (!renderTexture_ || !inputTexture) {
		return;
	}

	// このPassの出力先へ切り替えて、入力テクスチャを現在のエフェクトで描画する。
	PreDraw();
	inputTexture->Draw();
	PostDraw();
}

Engine::PostEffectData* Engine::PostEffectPass::GetPostEffectData()
{
	if (!renderTexture_) {
		return nullptr;
	}
	return renderTexture_->GetPostEffectData();
}

void Engine::PostEffectPass::PreDraw()
{
	// ポストエフェクトは画面全体の2D描画なので、深度は使わずRTVだけを設定する。
	barrier_->TransitionResource(renderTexture_->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = renderTexture_->GetRTVHandle();
	command_->GetList()->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

	float clearColor[] = {
		renderTexture_->GetClearColor().x,
		renderTexture_->GetClearColor().y,
		renderTexture_->GetClearColor().z,
		renderTexture_->GetClearColor().w
	};
	command_->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	viewPort_->SettingViewport();
	scissorRect_->SettingScissorRect();
}

void Engine::PostEffectPass::PostDraw()
{
	barrier_->TransitionResource(renderTexture_->GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void Engine::PostEffectPipeline::Initialize(DXGIDevice* dxgiDevice, Command* command, SrvManager* srvManager,
	RtvManager* rtvManager, RenderingCommon* renderingCommon,
	DepthStencil* depthStencil, Barrier* barrier,
	ScissorRect* scissorRect, ViewPort* viewPort)
{
	dxgiDevice_ = dxgiDevice;
	command_ = command;
	srvManager_ = srvManager;
	rtvManager_ = rtvManager;
	renderingCommon_ = renderingCommon;
	depthStencil_ = depthStencil;
	barrier_ = barrier;
	scissorRect_ = scissorRect;
	viewPort_ = viewPort;
}

Engine::PostEffectPass* Engine::PostEffectPipeline::AddPass(const std::string& name, PostEffectType type, bool use)
{
	return CreatePass(name, type, use);
}

Engine::PostEffectPass* Engine::PostEffectPipeline::AddEffect(const std::string& name, PostEffectBlockType type, bool use)
{
	switch (type) {
	case PostEffectBlockType::kCopy:
		return AddPass("Copy_" + name, PostEffectType::kCopy, use);
	case PostEffectBlockType::kGrayScale:
		return AddPass("GrayScale_" + name, PostEffectType::kGrayScale, use);
	case PostEffectBlockType::kSepia:
		return AddPass("Sepia_" + name, PostEffectType::kSepia, use);
	case PostEffectBlockType::kVignette:
		return AddPass("Vignette_" + name, PostEffectType::kVignette, use);
	case PostEffectBlockType::kSmoothing:
		return AddPass("Smoothing_" + name, PostEffectType::kSmoothing, use);
	case PostEffectBlockType::kGaussian:
		return AddPass("Gaussian_" + name, PostEffectType::kGaussian, use);
	case PostEffectBlockType::kOitline:
		return AddPass("Outline_" + name, PostEffectType::kOitline, use);
	case PostEffectBlockType::kRadialBlur:
		return AddPass("RadialBlur_" + name, PostEffectType::kRadialBlur, use);
	case PostEffectBlockType::kDissolve:
		return AddPass("Dissolve_" + name, PostEffectType::kDissolve, use);
	case PostEffectBlockType::kRandom:
		return AddPass("Random_" + name, PostEffectType::kRandom, use);
	case PostEffectBlockType::kBloom:
		AddBloom(name, use);
		return passes_.empty() ? nullptr : passes_.back().get();
	default:
		return nullptr;
	}
}

void Engine::PostEffectPipeline::AddBloom(const std::string& name, bool use)
{
	// BloomCombineは直前のGaussian結果に加えて、最初のBrightPass結果も参照する。
	PostEffectPass* brightPass = AddPass("BrightPass_" + name, PostEffectType::kBloom, use);
	AddPass("Gaussian_" + name, PostEffectType::kGaussian, use);
	PostEffectPass* combine = AddPass("BloomCombine_" + name, PostEffectType::kBloomCombine, use);
	if (combine) {
		combine->SetOtherInputPass(brightPass);
	}
}

void Engine::PostEffectPipeline::Clear()
{
	passes_.clear();
}

void Engine::PostEffectPipeline::Update(Camera* camera)
{
	for (auto& pass : passes_) {
		pass->Update(camera);
	}
}

Engine::RenderTexture* Engine::PostEffectPipeline::Execute(RenderTexture* sourceTexture)
{
	if (!sourceTexture) {
		return nullptr;
	}

	std::sort(passes_.begin(), passes_.end(),
		[](const std::unique_ptr<PostEffectPass>& a, const std::unique_ptr<PostEffectPass>& b) {
			return a->GetIndex() < b->GetIndex();
		});

	RenderTexture* currentTexture = sourceTexture;
	for (size_t i = 0; i < passes_.size(); ++i) {
		PostEffectPass* pass = passes_[i].get();
		pass->SetIndex(static_cast<uint32_t>(i));

		if (!pass->GetUse()) {
			continue;
		}

		// 複数入力パス用に、追加で参照するSRVを描画直前に更新する。
		PostEffectPass* otherInputPass = pass->GetOtherInputPass();
		if (otherInputPass) {
			RenderTexture* otherTexture = otherInputPass->GetRenderTexture();
			if (otherTexture) {
				pass->GetRenderTexture()->SetOtherSrvIndex(otherTexture->GetSrvIndex());
			}
		}

		pass->Draw(currentTexture);
		currentTexture = pass->GetRenderTexture();
	}

	return currentTexture;
}

Engine::PostEffectPass* Engine::PostEffectPipeline::GetPass(size_t index)
{
	if (index >= passes_.size()) {
		return nullptr;
	}
	return passes_[index].get();
}

Engine::PostEffectPass* Engine::PostEffectPipeline::FindPass(const std::string& name)
{
	for (auto& pass : passes_) {
		if (pass->GetName() == name) {
			return pass.get();
		}
	}
	return nullptr;
}

std::vector<Engine::PostEffectPass*> Engine::PostEffectPipeline::GetPasses()
{
	std::vector<PostEffectPass*> rawPtrs;
	rawPtrs.reserve(passes_.size());
	for (auto& pass : passes_) {
		rawPtrs.push_back(pass.get());
	}
	return rawPtrs;
}

Engine::PostEffectPass* Engine::PostEffectPipeline::CreatePass(const std::string& name, PostEffectType type, bool use)
{
	auto pass = std::make_unique<PostEffectPass>();
	pass->Initialize(dxgiDevice_, command_, srvManager_, rtvManager_, renderingCommon_,
		depthStencil_, barrier_, scissorRect_, viewPort_, name, type);
	pass->SetUse(use);
	pass->SetIndex(static_cast<uint32_t>(passes_.size()));

	PostEffectPass* result = pass.get();
	passes_.push_back(std::move(pass));
	return result;
}
