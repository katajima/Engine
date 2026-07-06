#pragma once
#include "psoData.h"


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

		ShaderBlobSet shaderBlobs_; // コンパイル済みシェーダーの共通保持領域

		HRESULT hr_;


		PSRS computePSRS_; // コンピュート用ルートシグネチャとPSO

		ShaderFileSet shaderFiles_; // 全シェーダーステージの共通ファイルパス
	public:
		void SetShederCompute(D3D12_COMPUTE_PIPELINE_STATE_DESC& graphicsPipeline);
	};
}
