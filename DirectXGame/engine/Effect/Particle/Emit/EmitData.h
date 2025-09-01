#pragma once
#include "EmitShape.h"


// エミットに関するデータ
namespace EmitData {
	enum class SpawnType // 出現形状
	{
		kPoint,     // Point
		kAABB,		// AABB
		kSphere,	// Sphere

		kSegmentLine,		// Line
		kCornerLine,        // コーナーライン
		kSpline,			// スプライン
	};

	enum class EmitType
	{
		kRandom,   // ランダム
		
	};


	enum class DirectionType // 方向
	{
		kNone,			// なし
		kNormal,		// 法線方向
		kVelocity,		// 速度方向
		kInverse,		// 逆方向
		kRandom,		// ランダム
		kFixed,			// 固定方向
		kVelocityBase,  // 速度依存
	};


	enum class TopBottom { // スケール変更
		kTop,
		kBottom,
	};
	
	// 描画モード
	enum class RasterizerType
	{
		MODE_SOLID_BACK,
		MODE_SOLID_NONE,
	};
	// ブレンドモード
	enum class BlendType
	{
		MODE_ADD,
		MODE_SUBTRACT,
		MODE_MUlLIPLY,
	};

	// 各種フラグ
	struct IsFlag
	{
		bool usebillboard = true;								// ビルボードするか
		bool usebillboardY = false;								// Y軸
		bool billboardRotZ = false;								// z軸回転するか
		bool isAlpha = false;									// 透明にしていくか
		bool isLine = true;										// ライン描画するか
		bool isGravity = false;									// 重力を有効にするか
		bool isLifeTimeScale_ = false;							// ちっちゃくしていくか
		bool isRotateVelocity = false;							// 回転するか
		bool isLifeTimeVelocity = false;						// 速度
		bool isBounce = false;									// 跳ねるか
		bool isAcceleration = false;							// 加速度を付けるか
		bool isLineInterpolation = false;						// ライン補間するか	
	};

}





// エミッターのデータ
class EffectEmitData
{
public:
	Range <Vector4> colorRange;			// 色 (Vector3の範囲)
	Range <Vector3> sizeRange;			// 大きさ (floatの範囲)
	Range <Vector3> rotateRange;		// 回転 (floatの範囲)
	Range <Vector3> rotateVelocityRange;// 回転 (floatの範囲)
	Range <float> lifeTimeRange;		// 生存時間 (floatの範囲)

	Range <Vector3> velocityRange;		// 速度 (Vector3の範囲)
	Range <Vector3> accelerationRange;	// 加速度 (Vector3の範囲)
	
	Range <int> countRange;				// 個数 (intの範囲)
	int count = 1;						// 個数


public:
	bool isEmit = false;			// エミットするか
	bool isLoop = false;			// ループするか
	
private:

};

