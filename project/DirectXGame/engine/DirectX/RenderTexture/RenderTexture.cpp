#include "RenderTexture.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/Manager/RTV/RtvManeger.h"
#include "DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/base/WinApp/WinApp.h"

#include "DirectXGame/engine/Offscreen/RenderingCommon.h"
#include "DirectXGame/engine/Offscreen/Posteffect.h"

#include "imgui.h"

Engine::RenderTexture::~RenderTexture()
{
	rtvManager->DecAllocate();
}

void Engine::RenderTexture::Initialize(DXGIDevice* DXGIDevice, Command* command, SrvManager* srvManager, RtvManager* rvtManager, RenderingCommon* renderingCommon, const std::string name, PostEffectType type)
{
	this->dxgiDevice = DXGIDevice;			// デバイス
	this->command = command;					// コマンド
	this->srvManager = srvManager;			// SRV管理クラス
	this->rtvManager = rvtManager;			// RTV管理クラス
	this->renderingCommon = renderingCommon;	// レンダリング共通クラス


	CreateResourcePixel();
	CreateRTV();      // RTV作成
	CreateSRV();      // SRV作成

	name_ = name;

	type_ = type;

	postEffectData_ = std::make_unique<PostEffectData>();
	postEffectData_->Initialize(renderingCommon->GetDxCommon(),type_);
}

void Engine::RenderTexture::Update()
{
	renderingCommon->SetCamera(camera);
	postEffectData_->SetCamera(camera);
#ifdef _DEBUG

	if (ImGui::TreeNode(name_.c_str())) {
		postEffectData_->UpdateImgui();
		ImGui::TreePop(); // <- 対応する TreePop を忘れずに！
	}
#endif // _DEBUG
}

void Engine::RenderTexture::Draw()
{
	renderingCommon->DrawRender(type_, srvIndex_, otherSrvIndex_);
	postEffectData_->DrawRender();
}

Vector4 Engine::RenderTexture::GetClearColor() const
{
	return clearColor_;
}

D3D12_CPU_DESCRIPTOR_HANDLE Engine::RenderTexture::GetRTVHandle()
{
	return rtvManager->GetCPUDescriptorHandle(rtvIndex_);
}

ID3D12Resource* Engine::RenderTexture::GetResource()
{
	return resource_.Get();
}

D3D12_GPU_DESCRIPTOR_HANDLE Engine::RenderTexture::GetSRVGPUHandle()
{
	return srvManager->GetGPUDescriptorHandle(srvIndex_);
}

D3D12_CPU_DESCRIPTOR_HANDLE Engine::RenderTexture::GetSRVCPUHandle()
{
	return srvManager->GetCPUDescriptorHandle(srvIndex_);
}

Engine::PostEffectData* Engine::RenderTexture::GetPostEffectData()
{
	return postEffectData_.get();
}

void Engine::RenderTexture::CreateResource()
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
	hr_ = dxgiDevice->GetDevice()->CreateCommittedResource(
		&heapProperties,								// Heapの設定
		D3D12_HEAP_FLAG_NONE,							// Heapの特殊な設定。特になし。
		&resourceDesc,									// リソースの設定
		D3D12_RESOURCE_STATE_RENDER_TARGET,				// これから描画することを前提としたTextureなので、RenderTargetとして使うことから始める
		&clearValue,									// Clear最適値、ClearRenderをこの色でクリアするようにする、最適化されているので高速である。
		IID_PPV_ARGS(&resource_)
	);
	assert(SUCCEEDED(hr_));
}

void Engine::RenderTexture::CreateResourcePixel()
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
	hr_ = dxgiDevice->GetDevice()->CreateCommittedResource(
		&heapProperties,								// Heapの設定
		D3D12_HEAP_FLAG_NONE,							// Heapの特殊な設定。特になし。
		&resourceDesc,									// リソースの設定
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,		// これから描画することを前提としたTextureなので、RenderTargetとして使うことから始める
		&clearValue,									// Clear最適値、ClearRenderをこの色でクリアするようにする、最適化されているので高速である。
		IID_PPV_ARGS(&resource_)
	);
	assert(SUCCEEDED(hr_));
}


void Engine::RenderTexture::CreateRTV()
{
	// インデックスを割り当て
	rtvIndex_ = rtvManager->Allocate();
	// RTVを作成
	rtvManager->CreateRTV(rtvIndex_, resource_.Get());


}



void Engine::RenderTexture::CreateSRV()
{
	// インデックス割りて
	srvIndex_ = srvManager->Allocate();
	// SRVを作成
	DirectX::TexMetadata matadata{};
	matadata.format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	matadata.mipLevels = 1;
	srvManager->CreateSRVforTexture2D(srvIndex_, resource_.Get(), matadata);
}
