#pragma once
#include <windows.h>
#include<cstdint>
#include "DirectXGame/engine/Base/pch.h"


namespace Engine {
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
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// メッセージの処理
		/// </summary>
		bool ProcessMessage();

		/// ウィンドウハンドルの取得
		static HWND GetHwnd() { return hwnd; }
		/// <summary>
		/// インスタンス取得
		/// </summary>
		static HINSTANCE GetHInstance() { return wc.hInstance; }
		/// <summary>
		/// 画面サイズ取得(横)
		/// </summary>
		static int32_t GetClientWidth(bool isFull = true) {
			if (isFullscreen && isFull) {
				return fullscreenWidth;
			}
			else {
				return kClientWidth;
			}
		}
		/// <summary>
		/// 画面サイズ取得(縦)
		/// </summary>
		static int32_t GetClientHeight(bool isFull = true) {
			if (isFullscreen && isFull) {
				return fullscreenHeight;
			}
			else {
				return kClientHeight;
			}
		}
		/// <summary>
		/// 画面のウィンドウからフルスクリーンの比率(横)
		/// </summary>
		static float GetCalculateWindowToFullscreenScaleRatioWidth() {
			if (isFullscreen) {
				return static_cast<float>(kClientWidth) / static_cast<float>(fullscreenWidth);
			}
			else {
				return 1.0f;
			}
		}
		/// <summary>
		/// 画面のウィンドウからフルスクリーンの比率(縦)
		/// </summary>
		static float GetCalculateWindowToFullscreenScaleRatioHeight() {
			if (isFullscreen) {
				return static_cast<float>(kClientHeight) / static_cast<float>(fullscreenHeight);
			}
			else {
				return 1.0f;
			}
		}
		/// <summary>
		/// 終了させる
		/// </summary>
		static void IsFinish() { isFinish_ = true; }

		//クライアント領域のサイズ
		static const int32_t kClientWidth = 1280;
		static const int32_t kClientHeight = 720;

		inline static int32_t fullscreenWidth = 0;
		inline static int32_t fullscreenHeight = 0;

		inline static float ratioWidth = 0;
		inline static float ratioHeight = 0;


		inline static bool isFinish_ = false;

		/// <summary>
		/// フルスクリーン設定
		/// </summary>
		static void ToggleFullscreen();
		/// <summary>
		/// フルスクリーンか
		/// </summary>
		static bool IsFullscreen() { return isFullscreen; }
		/// <summary>
		/// スワップチェーン設定
		/// </summary>
		static void SetSwapChain(SwapChain* sc) { swapChain = sc; }
	private:
		inline static HWND hwnd = nullptr; // ウィンドウハンドル
		inline static WNDCLASS wc{};       // ウィンドウクラス
		inline static bool isFullscreen = false;       // 状態フラグ
		inline static WINDOWPLACEMENT wpPrev = {};     // 復帰用に保存
		inline static SwapChain* swapChain = nullptr; // 静的に保持
	};
}
