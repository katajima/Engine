#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>

namespace Engine {
	// 3Dモデル共通部分
	class Input;
	class DirectXCommon;

	// カメラ共通クラス
/// <summary>
/// CameraCommonを管理・実装するクラス。
/// </summary>
	class CameraCommon
	{
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* dxCommon);
		/// <summary>
		/// DxCommon取得
		/// </summary>
		DirectXCommon* GetDxCommon() const { return dxCommon; }
		/// <summary>
		/// インプット設定
		/// </summary>
		void SetInput(Input* input) { this->input = input; };
		/// <summary>
		/// インプット取得
		/// </summary>
		Input* GetInput() { return input; };

	private:
		DirectXCommon* dxCommon = nullptr;

		Input* input = nullptr;
	};
}