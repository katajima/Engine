#pragma once
#include "PSOFunction.h"


#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFunctions.h"

namespace Engine {
	// 
	enum class PSOType {
		UvInterpolation_MODE_SOLID_BACK,		// UV補間ありソリッド(背面カリング)
		NoUvInterpolation_MODE_SOLID_BACK,		// UV補間なしソリッド(背面カリング)
		UvInterpolation_MODE_WIREFRAME_BACK,	// UV補間ありワイヤーフレーム(背面カリング)
		NoUvInterpolation_MODE_WIREFRAME_BACK,	// UV補間なしワイヤーフレーム(背面カリング)


		UvInterpolation_MODE_SOLID_NONE,		// UV補間ありソリッド(カリングなし)
		NoUvInterpolation_MODE_SOLID_NONE,		// UV補間なしソリッド(カリングなし)
		UvInterpolation_MODE_WIREFRAME_NONE,	// UV補間ありワイヤーフレーム(カリングなし)
		NoUvInterpolation_MODE_WIREFRAME_NONE,	// UV補間なしワイヤーフレーム(カリングなし)

		Transparent,							// 透明物
		ShadowMap,								// シャドウマップ用の深度描画
	};


	// ラスタライザタイプ
	enum class RasterizerType {
		MODE_SOLID_BACK,		// 背面カリング
		MODE_WIREFRAME_BACK,	// 背面カリングワイヤーフレーム
		MODE_SOLID_NONE,		// カリングなし
		MODE_WIREFRAME_NONE,	// カリングなしワイヤーフレーム
	};

	// ブレンドモードタイプ
	enum class BlendType
	{
		MODE_ADD,
		MODE_SUBTRACT,
		MODE_MUlLIPLY,
	};



	// PSOキー
	struct GraphicsPSOKey {
		bool isUvInterpolation_ = true; // UV補間するか
		bool isTransparent = false;		// 透過物か
		RasterizerType rasterizerType = RasterizerType::MODE_SOLID_BACK;
		BlendType blendType = BlendType::MODE_ADD;

	};


	// パイプラインステート＋ルートシグネチャ
	struct PSRS {
		// グラフィックス・コンピュートの両方で使用するルートシグネチャ
		Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
		// グラフィックス・コンピュートの両方で使用するパイプラインステート
		Microsoft::WRL::ComPtr < ID3D12PipelineState> pipelineState;
	};

	// 各シェーダーステージのファイルパスを共通管理する構造体
	struct ShaderFilePath {
		std::wstring filePath; // コンパイル対象となるシェーダーファイルのパス
	};

	// グラフィックス・コンピュートで使用するシェーダーファイル一式
	struct ShaderFileSet {
		ShaderFilePath pixel;         // ピクセルシェーダー
		ShaderFilePath vertex;        // 頂点シェーダー
		ShaderFilePath domain;        // ドメインシェーダー
		ShaderFilePath hull;          // ハルシェーダー
		ShaderFilePath geometry;      // ジオメトリシェーダー
		ShaderFilePath amplification; // アンプリフィケーションシェーダー
		ShaderFilePath mesh;          // メッシュシェーダー
		ShaderFilePath compute;       // コンピュートシェーダー
	};

	// コンパイル済みシェーダーを一時保持する共通構造体
	struct ShaderBlobSet {
		Microsoft::WRL::ComPtr < IDxcBlob> PS; // ピクセルシェーダー
		Microsoft::WRL::ComPtr < IDxcBlob> VS; // 頂点シェーダー
		Microsoft::WRL::ComPtr < IDxcBlob> DS; // ドメインシェーダー
		Microsoft::WRL::ComPtr < IDxcBlob> HS; // ハルシェーダー
		Microsoft::WRL::ComPtr < IDxcBlob> GS; // ジオメトリシェーダー
		Microsoft::WRL::ComPtr < IDxcBlob> AS; // アンプリフィケーションシェーダー
		Microsoft::WRL::ComPtr < IDxcBlob> MS; // メッシュシェーダー
		Microsoft::WRL::ComPtr < IDxcBlob> CS; // コンピュートシェーダー
	};
}
