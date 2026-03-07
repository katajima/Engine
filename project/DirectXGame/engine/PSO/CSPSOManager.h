#pragma once
#include "PSOFanction.h"


#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFunctions.h"


namespace Engine {
	// 前方宣言
	class Command;
	class DXGIDevice;
	class DXCCompiler;

	/// <summary>
	/// コンピュートシェーダのPSOマネージャークラス
	/// </summary>
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

		// ルートパラメータ設定
		void SetRootSignature(D3D12_ROOT_PARAMETER* rootParameter, UINT numRootParameters);
		// コンピュートパイプライン生成
		void ComputePipelineState();
		// コンピュートパイプライン設定
		void PreComputePSRS();
		// シェーダーファイル設定
		void SetShaderFileName(std::wstring filename);

	private:
		Command* command = nullptr;
		DXGIDevice* dxgiDevice = nullptr;
		DXCCompiler* dxcCompiler = nullptr;

		// ファイル名
		struct fileName {
			std::wstring filePach;
		};

		// シェーダーファイル
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
}