#pragma once
#include "PSOFanction.h"


#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"


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
	////ルートシグネチャ
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
	//// グラフィックスパイプラインステート
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState;
};