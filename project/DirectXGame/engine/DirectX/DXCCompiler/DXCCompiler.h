#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include <wrl.h>
using namespace Microsoft::WRL;

#include "DirectXGame/engine/base/Logger.h"
#include "DirectXGame/engine/Utility/StringUtility.h"


namespace Engine {
	/// <summary>
	/// DXコンパイラクラス
	/// </summary>
	class DXCCompiler
	{
	public:
		DXCCompiler() = default;
		~DXCCompiler() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		////------CompileShader------////
		Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
			//CompileShaderするShaderファイルへのパス
			const std::wstring& filePach,
			//CompileShaderに使用するProfile
			const wchar_t* profile);

	private:


	private:
		// DXユーティル
		Microsoft::WRL::ComPtr<IDxcUtils> mDxcUtils;
		// インクルードハンドラー
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> mIncludeHandler;
		// DXCコンパイル
		Microsoft::WRL::ComPtr<IDxcCompiler3> mDxcCompiler;
		// エラー判別
		HRESULT hr_ = S_OK;
	};
}
