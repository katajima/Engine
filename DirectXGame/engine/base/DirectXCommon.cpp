#include "DirectXCommon.h"
#include<format>
#include<cassert>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#include<vector>
#include"externals/imgui/imgui.h"
#include"externals/imgui/imgui_impl_dx12.h"
#include"externals/imgui/imgui_impl_win32.h"
using namespace Microsoft::WRL;

#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"
#include"TextureManager.h"

#include"RenderingCommon.h"

#include "DirectXGame/engine/Manager/RTV/RtvManeger.h"

void DirectXCommon::Intialize(WinApp* winApp) {
    
    InitializeFixFPS();
   
	DXGIDevice_->Initialize();
	command_->Initialize(DXGIDevice_.get()); // コマンド
	viewPort_->Initialize(command_.get());   // ビューポート
	scissorRect_->Initialize(command_.get());// シザー
	fence_->Initialize(DXGIDevice_.get(), command_.get()); // フェンス
	dxcCompiler_->Initialize(); // コンパイル
	srvManager_->Initialize(DXGIDevice_.get(), command_.get()); // SRV
	uavManager_->Initialize(DXGIDevice_.get(), command_.get()); // UAV
	rtvManager_->Initialize(DXGIDevice_.get(), command_.get()); // RTV
	swapChain_->Initialize(winApp,DXGIDevice_.get(),command_.get(), rtvManager_.get()); // スワップチェーン
	

    CreateDepthBuffer(); // 深度
    CreateDescriptorHeap(); // ディスクプリタヒープ

	

    CreateRenderTargets(); //レンダーターゲット
    InitializeDepthStencilView(); // デプス
   
}

#pragma region Draw

void DirectXCommon::PreDrawOffscreen() {
	//// オフスクリーン描画用の状態遷移
	//TransitionResourceState(renderTextureResource_.Get(),
	//	D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,  // 現在の状態
	//	D3D12_RESOURCE_STATE_RENDER_TARGET);          // 遷移後の状態

	//// 描画先の設定
	//D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//command_->GetList()->OMSetRenderTargets(1, &rtvTexHandle, false, &dsvHandle);

	//// レンダーターゲットと深度バッファをクリア
	//float clearColor[] = { 1.0f, 0.0f, 0.0f, 1.0f }; // 任意のクリアカラー（赤）
	//command_->GetList()->ClearRenderTargetView(rtvTexHandle, clearColor, 0, nullptr);
	//command_->GetList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//
	//viewPort_->SettingViewport();
	//scissorRect_->SettingScissorRect();
}

void DirectXCommon::PostDrawOffscreen() {
	//// オフスクリーン描画後の状態遷移
	//TransitionResourceState(renderTextureResource_.Get(),
	//	D3D12_RESOURCE_STATE_RENDER_TARGET,          // 現在の状態
	//	D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE); // 次の状態
}


void DirectXCommon::PreDrawSwap() {
	TransitionResourceState(swapChain_->GetCurrentBackBufferResource(),
		D3D12_RESOURCE_STATE_PRESENT,  // 現在の状態
		D3D12_RESOURCE_STATE_RENDER_TARGET);  // 遷移後の状態

	
	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChain_->GetCurrentBackBufferRTVHandle();
	command_->GetList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };  // 任意のクリアカラー（青）
	command_->GetList()->ClearRenderTargetView(swapChain_->GetCurrentBackBufferRTVHandle(), clearColor, 0, nullptr);
	command_->GetList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// コマンドを積む
	viewPort_->SettingViewport();
	scissorRect_->SettingScissorRect();
}

void DirectXCommon::PostDrawSwap() {
	
	TransitionResourceState(swapChain_->GetCurrentBackBufferResource(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,  // 現在の状態
		D3D12_RESOURCE_STATE_PRESENT);  // 遷移後の状態



	// コマンド
	command_->KickCommand();

	// GPUに画面交換を通知
	swapChain_->Present();

	// フェンス
	fence_->WaitGPU();

	// FPS制限の更新
	UpdateFixFPS();


	command_->ResetCommand();

}


#pragma endregion // 描画処理




void DirectXCommon::Finalize()
{
	
}

void DirectXCommon::TransitionResourceState(ID3D12Resource* resource, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState)
{
	if (beforeState != afterState)
	{
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = resource;
		barrier.Transition.StateBefore = beforeState;
		barrier.Transition.StateAfter = afterState;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		command_->GetList()->ResourceBarrier(1, &barrier);  // 1はバリアの数
	}
}


/// <summary>
/// DSV
/// </summary>
D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetDSVCPUDescriptorHandle(uint32_t index)
{
	return GetCPUDescriptorHandle(dsvDescriptorHeap, desriptorSizeDSV, index);
}
D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetDSVGPUDescriptorHandle(uint32_t index)
{
	return GetGPUDescriptorHandle(dsvDescriptorHeap, desriptorSizeDSV, index);
}


void DirectXCommon::CreateRenderTargets()
{
	
	////レンダーテクスチャ用のRTV
	//rtvTexHandle = rtvManager_->GetCPUDescriptorHandle(rtvTexIndex);
	//const Vector4 kRenderTargetClearValue{ 1.0f, 0.0f, 0.0f, 1.0f };
	//renderTextureResource_ = rtvManager->CreateRenderTextureResource(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTargetClearValue);
	//rtvManager->CreateRTV(rtvTexIndex, renderTextureResource_.Get());

}


void DirectXCommon::CreateDescriptorHeap()
{
	// DescriptorSizeを取得しておく
	//desriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	desriptorSizeDSV = DXGIDevice_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	// DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないので、ShaderVisibleはfalse
	dsvDescriptorHeap = DXGIDevice_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
}

void DirectXCommon::CreateDepthBuffer()
{

	//生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = WinApp::GetClientWidth();  // Textureの幅
	resourceDesc.Height = WinApp::GetClientHeight(); // Textureの高さ
	resourceDesc.MipLevels = 1; // mipmapの数
	resourceDesc.DepthOrArraySize = 1; // 奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //　DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //VRAM上に作る


	//深度値のクリア
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f; // 1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット。Resourceと合わせる


	//Resourceの生成

	HRESULT hr = DXGIDevice_->GetDevice()->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特に無し
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
		&depthClearValue, // Clear最高値
		IID_PPV_ARGS(&depthStencilResource)); // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));

}

void DirectXCommon::InitializeDepthStencilView()
{
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2dTexture

	// DSVHeapの先頭にDSVを作る
	DXGIDevice_->GetDevice()->CreateDepthStencilView(depthStencilResource.Get(), &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

}

void DirectXCommon::InitializeFixFPS()
{
	//現在時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

void DirectXCommon::UpdateFixFPS()
{
	// 1/60秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	// 1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	// 現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	// 前回の記録からの経過時間を記録する
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60秒（よりわずかに短い時間）経っていない場合
	if (elapsed < kMinCheckTime) {
		//1 / 60秒経過するまで微小なスリープ繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			// 1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}

	}
	// 現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();

}

// CPUHandle
D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetCPUDescriptorHandle(Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
};

// GPUHandle
D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetGPUDescriptorHandle(Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

// Material用のResource作成関数
Microsoft::WRL::ComPtr < ID3D12Resource> DirectXCommon::CreateBufferResource(size_t sizeInBytes) {

	////------VertexResourceを生成する------////
	HRESULT hr;

	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; //UploadHeapを使う

	//頂点リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};

	//バッファリソーステクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes;// リソースのサイズ。今回はVector4を3頂点分

	//バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;

	//バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//実際に頂点リソースを作る
	Microsoft::WRL::ComPtr < ID3D12Resource> resource = nullptr;

	hr = DXGIDevice_->GetDevice()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&resource));

	assert(SUCCEEDED(hr));

	return resource;
}


Microsoft::WRL::ComPtr <ID3D12Resource> DirectXCommon::CreateTextureResource(const DirectX::TexMetadata& metadata)
{
	//1. metadataを基にResourceの設定

	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width); // Textureの幅
	resourceDesc.Height = UINT(metadata.height); // Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels); // mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き or 配列Textureの配列数
	resourceDesc.Format = metadata.format; //TextureのFormat
	resourceDesc.SampleDesc.Count = 1; //サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数。普段使っているのは2次元


	//2. 利用するHeapの設定

	//利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケースがある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //細かい設定を行う
	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; //プロセッサの近くに配置


	//3. Resourceを生成する

	Microsoft::WRL::ComPtr <ID3D12Resource> resource = nullptr;
	HRESULT hr = DXGIDevice_->GetDevice()->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST, // データ転送される設定
		nullptr, // Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource)); // 作成するResourceのポインタへのポインタ

	assert(SUCCEEDED(hr));

	return resource;
}

//データを転送するUploadTextureData関数を作る
[[nodiscard]]
Microsoft::WRL::ComPtr < ID3D12Resource> DirectXCommon::UploadTextureData(Microsoft::WRL::ComPtr < ID3D12Resource> texture, const DirectX::ScratchImage& mipImages)
{
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(DXGIDevice_->GetDevice(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr < ID3D12Resource> intermediateResource = CreateBufferResource(intermediateSize);
	UpdateSubresources(command_->GetList(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
	// Textureへの転送後は利用できるよう、D3D12_RESORCE_STATE_COPYからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	command_->GetList()->ResourceBarrier(1, &barrier);
	return intermediateResource;
}


void DirectXCommon::CreateRenderTexture() {
	////const Vector4 kRenderTargetClearValue{ 1.0f, 0.0f, 0.0f, 1.0f };
	////renderTextureResource_ = CreateRenderTextureResource(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTargetClearValue);
	////device->CreateRenderTargetView(renderTextureResource_.Get(), &rtvTexDesc_, rtvTexHandle);
	//
	//D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	//renderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//renderTextureSrvDesc.Texture2D.MipLevels = 1;
	//
	//if (index == 0) {
	//	index = SrvManager::GetInstance()->Allocate();
	//}
	//DXGIDevice_->GetDevice()->CreateShaderResourceView(
	//	renderTextureResource_.Get(), &renderTextureSrvDesc, SrvManager::GetInstance()->GetCPUDescriptorHandle(index));
}

Microsoft::WRL::ComPtr<IDxcBlob> DirectXCommon::CompileShader(const std::wstring& filePach, const wchar_t* profile)
{
	return dxcCompiler_->CompileShader(filePach, profile);
};
