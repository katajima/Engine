#include"WinApp.h"

#pragma comment(lib,"winmm.lib")

#include"externals/imgui/imgui.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"

#include "DirectXGame/engine/DirectX/SwapChain/SwapChain.h"


LRESULT CALLBACK Engine::WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{

	// ImGui用のウィンドウプロシージャ呼び出し
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:

		PostQuitMessage(0);
	case WM_KEYDOWN:
		if (wparam == VK_F11) {  // F11で切替
			ToggleFullscreen();
		}
		return 0;
	}
	if (isFnish_) {

		//OSに応じて、アプリの終了を伝える
		PostQuitMessage(0);
	}
	//標準メッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void Engine::WinApp::Initialize()
{
	// システムタイマーの分解能を上げる
	timeBeginPeriod(1);

	//COMの初期化
	CoInitializeEx(0, COINIT_MULTITHREADED);
	//ウィンドウプロシージャ
	wc.lpfnWndProc = WindowProc;

	//ウィンドウクラス名(何でもいい)
	wc.lpszClassName = L"CG2WindowClass";

	//インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);

	//カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	//ウィンドウクラスを登録する
	RegisterClass(&wc);



	//ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	//クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	hwnd = CreateWindow(

		wc.lpszClassName,      // 利用するクラス
		L"LE3A_10_カタジマロウノシン",	// タイトルバーの文字 (何でもいい)
		WS_OVERLAPPEDWINDOW,   // よく見るウィンドウスタイル
		CW_USEDEFAULT,		   // 表示X座標 (windousに任せる)
		CW_USEDEFAULT,		   // 表示Y座標 (WindowsOSに任せる)
		wrc.right - wrc.left,  // ウィンドウ横幅
		wrc.bottom - wrc.top,  // ウィンドウ縦幅
		nullptr,			   // 親ウィンドウハンドル
		nullptr,			   // メニューハンドル
		wc.hInstance,		   // インスタンスハンドル
		nullptr);			   // オプション

	//ウィンドウを表示する
	ShowWindow(hwnd, SW_SHOW);

}



void Engine::WinApp::Finalize()
{
	CloseWindow(hwnd);
	CoUninitialize();
}

bool Engine::WinApp::ProcessMessage()
{
	MSG msg{};

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT)
	{
		return true;
	}

	return false;
}

void Engine::WinApp::ToggleFullscreen()
{
	if (!hwnd || !swapChain) return;

	if (!isFullscreen) {
		// 現在のウィンドウ位置を保存
		wpPrev.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(hwnd, &wpPrev);

		// モニター解像度を取得
		HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(hMonitor, &mi);

		// 枠を消す
		SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);

		// 全画面に拡大
		SetWindowPos(hwnd, HWND_TOP,
			mi.rcMonitor.left,
			mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_FRAMECHANGED | SWP_NOOWNERZORDER);

		isFullscreen = true;


		fullscreenWidth = mi.rcMonitor.right - mi.rcMonitor.left;
		fullscreenHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;

#ifdef _DEBUG
		swapChain->Resize(fullscreenWidth, fullscreenHeight);
#endif // DEBUG

#ifndef _DEBUG
		swapChain->Resize(kClientWidth, kClientHeight);
#endif // !_DEBUG



	}
	else {
		// 元のスタイルに戻す
		SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);

		// 保存しておいた位置に戻す
		SetWindowPlacement(hwnd, &wpPrev);
		SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
			SWP_FRAMECHANGED | SWP_NOOWNERZORDER);

		isFullscreen = false;

		swapChain->Resize(kClientWidth, kClientHeight);
	}
}
