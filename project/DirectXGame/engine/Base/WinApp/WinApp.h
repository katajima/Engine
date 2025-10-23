#pragma once
#include <windows.h>
#include<cstdint>
#include "DirectXGame/engine/Base/pch.h"


// WindowsAPI
class SwapChain;

/// <summary>
/// ウィンドウクラス
/// </summary>
class WinApp
{
public: // 静的メンバ関数
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
public: // メンバ関数
	// 初期化
	void Initialize();
	// 終了
	void Finalize();

	// メッセージの処理
	bool ProcessMessage();

	/// ウィンドウハンドルの取得
	static HWND GetHwnd() { return hwnd; }
	// インスタンス取得
	static HINSTANCE GetHInstance() { return wc.hInstance; }
	// 画面サイズ取得(横)
	static int32_t GetClientWidth(bool isFull = true) {
		if (isFullscreen && isFull) {
			return fullscreenWidth;
		}
		else {
			return kClientWidth;
		}
	}
	// 画面サイズ取得(縦)
	static int32_t GetClientHeight(bool isFull = true) {
		if (isFullscreen && isFull) {
			return fullscreenHeight;
		}
		else {
			return kClientHeight;
		}
	}
	// 画面のウィンドウからフルスクリーンの比率(横)
	static float GetCalculateWindowToFullscreenScaleRatioWidth() {
		if (isFullscreen) {
			return static_cast<float>(kClientWidth) / static_cast<float>(fullscreenWidth);
		}
		else {
			return 1.0f;
		}
	}
	// 画面のウィンドウからフルスクリーンの比率(縦)
	static float GetCalculateWindowToFullscreenScaleRatioHeight() {
		if (isFullscreen) {
			return static_cast<float>(kClientHeight) / static_cast<float>(fullscreenHeight);
		}
		else {
			return 1.0f;
		}
	}
	// 終了させる
	static void IsFinish() { isFnish_ = true; }

	//クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	inline static int32_t fullscreenWidth = 0;
	inline static int32_t fullscreenHeight = 0;

	inline static float ratioWidth = 0;
	inline static float ratioHeight = 0;


	inline static bool isFnish_ = false;

	// フルスクリーン設定
	static void ToggleFullscreen();
	// フルスクリーンか
	static bool IsFullscreen() { return isFullscreen; }
	// スワップチェーン設定
	static void SetSwapChain(SwapChain* sc) { swapChain = sc; }
private:
	inline static HWND hwnd = nullptr; // ウィンドウハンドル
	inline static WNDCLASS wc{};       // ウィンドウクラス
	inline static bool isFullscreen = false;       // 状態フラグ
	inline static WINDOWPLACEMENT wpPrev = {};     // 復帰用に保存
	inline static SwapChain* swapChain = nullptr; // 静的に保持
};

