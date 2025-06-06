#pragma once
#include "PSOFanction.h"


#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"

class Command;
class DXGIDevice;
class DXCCompiler;

class CSPSOManager
{
public:
	// パイプラインステート＋ルートシグネチャ
	struct PSRS {
		////ルートシグネチャ
		Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
		//// グラフィックスパイプラインステート
		Microsoft::WRL::ComPtr < ID3D12PipelineState> computePipelineState;
	};

	// 初期化
	void Initialize(Command* command, DXGIDevice* DXGIDevice, DXCCompiler* dxcCompiler);


	void SetRootSignature(D3D12_ROOT_PARAMETER* rootParameter, UINT numRootParameters);

	void ComputePipelineState();

	void PreComputePSRS();

	void SetShaderFileName(std::wstring filename);

private:
	Command* command_;
	DXGIDevice* DXGIDevice_;
	DXCCompiler* dxcCompiler_;


	struct fileName {
		std::wstring filePach;
	};


	struct ShaderFile
	{
		fileName commpute;
	};

	Microsoft::WRL::ComPtr < IDxcBlob> CS;

	HRESULT hr_;


	PSRS computePSRS_;

	ShaderFile shderFile_;
public:
	void SetShederCompute(D3D12_COMPUTE_PIPELINE_STATE_DESC& graphicsPipeline);
};