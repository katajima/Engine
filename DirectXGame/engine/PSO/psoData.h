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

