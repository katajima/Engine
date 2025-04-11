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
#include "DirectXGame/engine/base/TextureManager.h"

#include"DirectXGame/engine/base/RenderingCommon.h"

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
	dsvManager_->Initialize(DXGIDevice_.get(), command_.get()); // DSV
	swapChain_->Initialize(winApp,DXGIDevice_.get(),command_.get(), rtvManager_.get()); // スワップチェーン
	depthStencil_->Initialize(DXGIDevice_.get(),command_.get(),dsvManager_.get()); // デプスステンシル     
	renderTexture_->Initialize(DXGIDevice_.get(), command_.get(), srvManager_.get(), rtvManager_.get()); // レンダーテクスチャ
	barrier_->Initialize(command_.get(),swapChain_.get(),renderTexture_.get()); // バリア
	psoManager_->Initialize(command_.get(), DXGIDevice_.get(), dxcCompiler_.get());// PSOマネージャー

	textureManager_->Initialize(command_.get(),DXGIDevice_.get(),srvManager_.get()); // テクスチャマネージャー
	modelManager_->Initialize(command_.get(), DXGIDevice_.get(), srvManager_.get()); // モデルマネージャー
	


}

#pragma region Draw

void DirectXCommon::PreDrawOffscreen() {
	//// 描画先の設定
	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencil_->GetCPUHandleDepthStencilResorce();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChain_->GetCurrentBackBufferRTVHandle();
	command_->GetList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	//// レンダーターゲットと深度バッファをクリア
	float clearColor[] = { 1.0f, 0.0f, 0.0f, 1.0f }; // 任意のクリアカラー（赤）
	command_->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	//command_->GetList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//
	viewPort_->SettingViewport();
	scissorRect_->SettingScissorRect();
}

void DirectXCommon::PreDrawSwap() {
	
	// バリア
	barrier_->Pre();
	
	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencil_->GetCPUHandleDepthStencilResorce();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = swapChain_->GetCurrentBackBufferRTVHandle();
	command_->GetList()->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };  // 任意のクリアカラー（青）
	command_->GetList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	command_->GetList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// コマンドを積む
	viewPort_->SettingViewport();
	scissorRect_->SettingScissorRect();
}

void DirectXCommon::PostDrawSwap() {
	
	// バリア
	barrier_->Post();

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


Microsoft::WRL::ComPtr<IDxcBlob> DirectXCommon::CompileShader(const std::wstring& filePach, const wchar_t* profile)
{
	return dxcCompiler_->CompileShader(filePach, profile);
};
