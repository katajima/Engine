#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>

namespace Engine {
	// 3Dモデル共通部分
	class Input;
	class DirectXCommon;

	// カメラ共通クラス
	class CameraCommon
	{
	public:
		// 初期化
		void Initialize(DirectXCommon* dxCommon);
		// DxCommon取得
		DirectXCommon* GetDxCommon() const { return dxCommon; }
		// インプット設定
		void SetInput(Input* input) { this->input = input; };
		// インプット取得
		Input* GetInput() { return input; };

	private:
		DirectXCommon* dxCommon = nullptr;

		Input* input = nullptr;
	};
}