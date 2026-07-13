#include"D3DResourceLeakchecker.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxgidebug.h>
#include<wrl.h>

void Engine::D3DResourceLeakchecker::ReportLiveObjects()
{
	//リソースリークチェック
	Microsoft::WRL::ComPtr < IDXGIDebug> debug;

	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		// DebugLayer内部の管理オブジェクトを除外し、アプリ側のD3D12リソースだけを確認する。
		const DXGI_DEBUG_RLO_FLAGS flags = static_cast<DXGI_DEBUG_RLO_FLAGS>(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, flags);
	}
}

Engine::D3DResourceLeakchecker::~D3DResourceLeakchecker()
{
	// main終了時はプロセス終了処理に近く簡易レポートになりやすいため、Framework::Finalize側で明示的に確認する。
}
