#include "RenderTexture.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/Manager/RTV/RtvManeger.h"
#include "DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/base/WinApp.h"

#include "DirectXGame/engine/Offscreen/RenderingCommon.h"


#include "imgui.h"

void RenderTexture::Initialize(DXGIDevice* DXGIDevice, Command* command, SrvManager* srvManager, RtvManager* rvtManager, RenderingCommon* renderingCommon, const std::string name)
{
	DXGIDevice_ = DXGIDevice;
	command_ = command;
	srvManager_ = srvManager;
	rtvManager_ = rvtManager;
	renderingCommon_ = renderingCommon;


	//CreateResource(); // リソース作成
	CreateResourcePixel();
	CreateRTV();      // RTV作成
	CreateSRV();      // SRV作成

	//resource_->SetName(L"RenderTexture");

	name_ = name;

	type_ = RenderTexture::PostEffectType::kCopy;
}

void RenderTexture::Update()
{
	renderingCommon_->SetCamera(camera_);

#ifdef _DEBUG
	
	if (ImGui::TreeNode(name_.c_str())) {
		if (ImGui::Button("kCopy")) {
			type_ = RenderTexture::PostEffectType::kCopy;
		}
		if (ImGui::Button("kDissovle")) {
			type_ = RenderTexture::PostEffectType::kDissovle;
		}
		if (ImGui::Button("kGaussian")) {
			type_ = RenderTexture::PostEffectType::kGaussian;
		}
		if (ImGui::Button("kGrayScale")) {
			type_ = RenderTexture::PostEffectType::kGrayScale;
		}
		if (ImGui::Button("kOitline")) {
			type_ = RenderTexture::PostEffectType::kOitline;
		}
		if (ImGui::Button("kRadialBlur")) {
			type_ = RenderTexture::PostEffectType::kRadialBlur;
		}
		if (ImGui::Button("kRandom")) {
			type_ = RenderTexture::PostEffectType::kRandom;
		}
		if (ImGui::Button("kSepia")) {
			type_ = RenderTexture::PostEffectType::kSepia;
		}
		if (ImGui::Button("kSmoothing")) {
			type_ = RenderTexture::PostEffectType::kSmoothing;
		}
		if (ImGui::Button("kVignette")) {
			type_ = RenderTexture::PostEffectType::kVignette;
		}
		if (ImGui::Button("kBloom")) {
			type_ = RenderTexture::PostEffectType::kBloom;
		}
		if (ImGui::Button("kBloomCombin")) {
			type_ = RenderTexture::PostEffectType::kBloomCombin;
		}
		UpdateImgui();
		ImGui::TreePop(); // <- 対応する TreePop を忘れずに！
	}
#endif // _DEBUG


	
}

void RenderTexture::Draw(RenderTexture* renderTexture)
{
	switch (type_)
	{
	case RenderTexture::PostEffectType::kCopy:
		renderingCommon_->DrawCopyRender(srvIndex_);
		break;
	case RenderTexture::PostEffectType::kGrayScale:
		renderingCommon_->DrawGrayScaleRender(srvIndex_);
		break;
	case RenderTexture::PostEffectType::kSepia:
		renderingCommon_->DrawSepiaeRender(srvIndex_);
		break;
	case RenderTexture::PostEffectType::kVignette:
		renderingCommon_->DrawVignetteRender(srvIndex_);
		break;
	case RenderTexture::PostEffectType::kSmoothing:
		renderingCommon_->DrawSmoothingRender(srvIndex_);
		break;
	case RenderTexture::PostEffectType::kGaussian:
		renderingCommon_->DrawGaussianRender(srvIndex_);
		break;
	case RenderTexture::PostEffectType::kOitline:
		renderingCommon_->DrawOutlineRender(srvIndex_);
		break;
	case RenderTexture::PostEffectType::kRadialBlur:
		renderingCommon_->DrawRadialBlurRender(srvIndex_);
		break;
	case RenderTexture::PostEffectType::kDissovle:
		renderingCommon_->DrawDissovleRender(srvIndex_);
		break;
	case RenderTexture::PostEffectType::kRandom:
		renderingCommon_->DrawRandomRender(srvIndex_);
		break;
	case RenderTexture::PostEffectType::kBloom:
		renderingCommon_->DrawBloomRender(srvIndex_);
		break;
	case RenderTexture::PostEffectType::kBloomCombin:
		renderingCommon_->DrawBloomCombinRender(renderTexture->srvIndex_,srvIndex_);

		break;
	default:
		renderingCommon_->DrawCopyRender(srvIndex_);
		break;
	}
}

Vector4 RenderTexture::GetClearColor() const
{
	return clearColor_;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTexture::GetRTVHandle()
{
	return rtvManager_->GetCPUDescriptorHandle(rtvIndex_);
}

ID3D12Resource* RenderTexture::GetResource()
{
	return resource_.Get();
}



void RenderTexture::CreateResource()
{
	// リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(WinApp::kClientWidth);					// Textureの幅
	resourceDesc.Height = UINT(WinApp::kClientHeight);					// Textureの高さ
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;				// TextureのFormat
	resourceDesc.SampleDesc.Count = 1;									// サンプリングカウント。1固定
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;		// renderTargetとして利用可能にする
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// クリアカラーの設定
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	clearValue.Color[0] = clearColor_.x;
	clearValue.Color[1] = clearColor_.y;
	clearValue.Color[2] = clearColor_.z;
	clearValue.Color[3] = clearColor_.w;

	// リソースの作成
	resource_ = nullptr;
	hr_ = DXGIDevice_->GetDevice()->CreateCommittedResource(
		&heapProperties,								// Heapの設定
		D3D12_HEAP_FLAG_NONE,							// Heapの特殊な設定。特になし。
		&resourceDesc,									// リソースの設定
		D3D12_RESOURCE_STATE_RENDER_TARGET,				// これから描画することを前提としたTextureなので、RenderTargetとして使うことから始める
		&clearValue,									// Clear最適値、ClearRenderをこの色でクリアするようにする、最適化されているので高速である。
		IID_PPV_ARGS(&resource_)
	);
	assert(SUCCEEDED(hr_));
}

void RenderTexture::CreateResourcePixel()
{
	// リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(WinApp::kClientWidth);					// Textureの幅
	resourceDesc.Height = UINT(WinApp::kClientHeight);					// Textureの高さ
	resourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;				// TextureのFormat
	resourceDesc.SampleDesc.Count = 1;									// サンプリングカウント。1固定
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;		// renderTargetとして利用可能にする
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	// クリアカラーの設定
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	clearValue.Color[0] = clearColor_.x;
	clearValue.Color[1] = clearColor_.y;
	clearValue.Color[2] = clearColor_.z;
	clearValue.Color[3] = clearColor_.w;

	// リソースの作成
	resource_ = nullptr;
	hr_ = DXGIDevice_->GetDevice()->CreateCommittedResource(
		&heapProperties,								// Heapの設定
		D3D12_HEAP_FLAG_NONE,							// Heapの特殊な設定。特になし。
		&resourceDesc,									// リソースの設定
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,		// これから描画することを前提としたTextureなので、RenderTargetとして使うことから始める
		&clearValue,									// Clear最適値、ClearRenderをこの色でクリアするようにする、最適化されているので高速である。
		IID_PPV_ARGS(&resource_)
	);
	assert(SUCCEEDED(hr_));
}


void RenderTexture::CreateRTV()
{
	// インデックスを割り当て
	rtvIndex_ = rtvManager_->Allocate();
	// RTVを作成
	rtvManager_->CreateRTV(rtvIndex_, resource_.Get());


}



void RenderTexture::CreateSRV()
{
	// インデックス割りて
	srvIndex_ = srvManager_->Allocate();
	// SRVを作成
	DirectX::TexMetadata matadata{};
	matadata.format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	matadata.mipLevels = 1;
	srvManager_->CreateSRVforTexture2D(srvIndex_, resource_.Get(), matadata);
}

void RenderTexture::UpdateImgui()
{

	switch (type_)
	{
	case RenderTexture::PostEffectType::kCopy:
		renderingCommon_->UpdateImgui(RenderingCommon::PostEffectType::kCopy);
		break;
	case RenderTexture::PostEffectType::kGrayScale:
		renderingCommon_->UpdateImgui(RenderingCommon::PostEffectType::kGrayScale);
		break;
	case RenderTexture::PostEffectType::kSepia:
		renderingCommon_->UpdateImgui(RenderingCommon::PostEffectType::kSepia);
		break;
	case RenderTexture::PostEffectType::kVignette:
		renderingCommon_->UpdateImgui(RenderingCommon::PostEffectType::kVignette);
		break;
	case RenderTexture::PostEffectType::kSmoothing:
		renderingCommon_->UpdateImgui(RenderingCommon::PostEffectType::kSmoothing);
		break;
	case RenderTexture::PostEffectType::kGaussian:
		renderingCommon_->UpdateImgui(RenderingCommon::PostEffectType::kGaussian);
		break;
	case RenderTexture::PostEffectType::kOitline:
		renderingCommon_->UpdateImgui(RenderingCommon::PostEffectType::kOitline);
		break;
	case RenderTexture::PostEffectType::kRadialBlur:
		renderingCommon_->UpdateImgui(RenderingCommon::PostEffectType::kRadialBlur);
		break;
	case RenderTexture::PostEffectType::kDissovle:
		renderingCommon_->UpdateImgui(RenderingCommon::PostEffectType::kDissovle);
		break;
	case RenderTexture::PostEffectType::kRandom:
		renderingCommon_->UpdateImgui(RenderingCommon::PostEffectType::kRandom);
		break;
	case RenderTexture::PostEffectType::kBloom:
		renderingCommon_->UpdateImgui(RenderingCommon::PostEffectType::kBloom);
		break;
	case RenderTexture::PostEffectType::kBloomCombin:
		renderingCommon_->UpdateImgui(RenderingCommon::PostEffectType::kBloomCombin);
		break;
	default:
		break;
	}
}


