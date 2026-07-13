#pragma once
#include "EmitShape.h"


// エミットに関するデータ
namespace EmitData {
	// sin
	enum class EmitSin {
		UP,
		DOWN,
		LEFT,
		RIGHT
	};

	// 出る方法
	enum class EmitType
	{
		kRandom,	// ランダム
		kSurface,	// 表面
		kEdge,		// エッジ
	};

	// 方向
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

	// スケール変更
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
		MODE_MULTIPLY,
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
		bool isScaling_ = false;								// サイズを大きくするか
	};

}

// シェイプタイプ
enum class EmitterShapeType
{
	AABB,		// AABB
	SPHERE,		// 球
	CIRCLE,		// 円
	POINT,		// 点
	LINE,		// 線
	SPLINE,		// スプライン
	TRIANGLE,	// 三角形
	MESH,		// メッシュ
};


namespace Engine {
	// エミッターのデータ
	struct EffectEmitData {
		Range <Vector4> colorRange = { {1,1,1,1},{1,1,1,1} };					// 色 (Vector3の範囲)
		MedianWithRange <Vector3> size = { {1,1,1},{} };				// 大きさ (floatの範囲)
		MedianWithRange <Vector3> rotate = {};			// 回転 (floatの範囲)
		MedianWithRange <Vector3> rotateVelocity = {};	// 回転 (floatの範囲)
		MedianWithRange <float> lifeTime = {1,0};			// 生存時間 (floatの範囲)

		MedianWithRange <Vector3> velocity = {};		// 速度 (Vector3の範囲)
		MedianWithRange <Vector3> acceleration = {};	// 加速度 (Vector3の範囲)

		MedianWithRange <Vector3> sizeAmount = { {},{} };	// サイズ増減量 (Vector3の範囲)

		MedianWithRange <int> count = {1,0};				// 個数 (intの範囲)

		bool isEmit = false;			// エミットするか
		bool isLoop = false;			// ループするか
		bool isUniformSize = false;		// サイズ一律(X座標優先)
		bool isDirectionRotate = false;	// 方向によって回転を決める
		bool isNoise = false;
		int direction = 0; // 0↑、1→、2↓,3←
	};

	// エミッターのデータ(2d)
	struct EffectEmitData2d {
		Range <Vector4> colorRange;					// 色 (Vector3の範囲)
		MedianWithRange <Vector2> size;				// 大きさ (floatの範囲)
		MedianWithRange <float> rotate;				// 回転 (floatの範囲)
		MedianWithRange <float> rotateVelocity;		// 回転 (floatの範囲)
		MedianWithRange <float> lifeTime;			// 生存時間 (floatの範囲)

		MedianWithRange <Vector2> velocity;			// 速度 (Vector3の範囲)
		MedianWithRange <Vector2> acceleration;		// 加速度 (Vector3の範囲)

		MedianWithRange <int> count;				// 個数 (intの範囲)

		bool isEmit = false;			// エミットするか
		bool isLoop = false;			// ループするか
	};
}