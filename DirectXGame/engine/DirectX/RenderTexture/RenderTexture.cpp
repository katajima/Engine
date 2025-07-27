#include "RenderTexture.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/Manager/RTV/RtvManeger.h"
#include "DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/base/WinApp/WinApp.h"

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

	name_ = name;

	type_ = PostEffectType::kCopy;
}

void RenderTexture::Update()
{
	renderingCommon_->SetCamera(camera_);

#ifdef _DEBUG

	if (ImGui::TreeNode(name_.c_str())) {
		UpdateImgui();
		ImGui::TreePop(); // <- 対応する TreePop を忘れずに！
	}
#endif // _DEBUG



}

void RenderTexture::Draw()
{
	renderingCommon_->DrawRender(type_, srvIndex_, otherSrvIndex_);
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

D3D12_GPU_DESCRIPTOR_HANDLE RenderTexture::GetSRVGPUHandle()
{
	return srvManager_->GetGPUDescriptorHandle(srvIndex_);
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTexture::GetSRVCPUHandle()
{
	return srvManager_->GetCPUDescriptorHandle(srvIndex_);
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
	//if (ImGui::Button("kCopy")) {
	//	type_ = PostEffectType::kCopy;
	//}
	//if (ImGui::Button("kDissovle")) {
	//	type_ = PostEffectType::kDissovle;
	//}
	//if (ImGui::Button("kGaussian")) {
	//	type_ = PostEffectType::kGaussian;
	//}
	//if (ImGui::Button("kGrayScale")) {
	//	type_ = PostEffectType::kGrayScale;
	//}
	//if (ImGui::Button("kOitline")) {
	//	type_ = PostEffectType::kOitline;
	//}
	//if (ImGui::Button("kRadialBlur")) {
	//	type_ = PostEffectType::kRadialBlur;
	//}
	//if (ImGui::Button("kRandom")) {
	//	type_ = PostEffectType::kRandom;
	//}
	//if (ImGui::Button("kSepia")) {
	//	type_ = PostEffectType::kSepia;
	//}
	//if (ImGui::Button("kSmoothing")) {
	//	type_ = PostEffectType::kSmoothing;
	//}
	//if (ImGui::Button("kVignette")) {
	//	type_ = PostEffectType::kVignette;
	//}
	//if (ImGui::Button("kBloom")) {
	//	type_ = PostEffectType::kBloom;
	//}
	//if (ImGui::Button("kBloomCombin")) {
	//	type_ = PostEffectType::kBloomCombin;
	//}

	renderingCommon_->UpdateImgui(type_);

}


