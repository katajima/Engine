#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>

#include <wrl.h>
using namespace Microsoft::WRL;

#include "DirectXGame/engine/base/Logger.h"
#include "DirectXGame/engine/base/StringUtility.h"

class DXCCompiler
{
public:
	DXCCompiler() = default;
	~DXCCompiler() = default;

	// 初期化
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
	IDxcUtils* dxcUtils_;
	// インクルードハンドラー
	IDxcIncludeHandler* includeHandler_;
	// DXCコンパイル
	IDxcCompiler3* dxcCompiler_;
	// エラー判別
	HRESULT hr_;
};

