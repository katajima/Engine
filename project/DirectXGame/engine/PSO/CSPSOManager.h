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
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Command* command, DXGIDevice* DXGIDevice, DXCCompiler* dxcCompiler);

		/// <summary>
		/// ルートパラメータ設定
		/// </summary>
		void SetRootSignature(D3D12_ROOT_PARAMETER* rootParameter, UINT numRootParameters);
		/// <summary>
		/// コンピュートパイプライン生成
		/// </summary>
		void ComputePipelineState();
		/// <summary>
		/// コンピュートパイプライン設定
		/// </summary>
		void PreComputePSRS();
		/// <summary>
		/// シェーダーファイル設定
		/// </summary>
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
