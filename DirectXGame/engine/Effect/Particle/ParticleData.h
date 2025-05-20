#pragma once
#include "DirectXGame/engine/math/MathFanctions.h"
#include "DirectXGame/engine/struct/Structs3D.h"
#include "DirectXGame/engine/DirectX/Resource/StructuredBuffer.h"
#include "DirectXGame/engine/3d/Model/Model.h"

class Material;
class Primitive;

// GPUに送るデータ
struct ParticleForGPU
{
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

// パーティクル(一粒)
struct Particle
{
	Transform transform;
	Vector3 velocity;
	Vector3 acceleration;
	Vector4 color;
	float lifeTime;
	float currentTime;

	Transform strtTransform;
	Vector3 rotateVelocity;
	Matrix4x4 pre;
};

namespace ParticleData {
	enum class SpawnType // 出現形状
	{
		kPoint,     // Point
		kAABB,		// AABB
		kOBB,		// OBB
		kSphere,	// Sphere

		kSegmentLine,		// Line
		kCornerLine,        // コーナーライン
		kSpline,			// スプライン
	};
	enum class EmitType
	{
		kRandom,   // ランダム
	};
	enum class TopBottom { // スケール変更
		kTop,
		kBottom,
	};
	enum class RasterizerType
	{
		MODE_SOLID_BACK,
		MODE_SOLID_NONE,
	};

	enum class BlendType
	{
		MODE_ADD,
		MODE_SUBTRACT,
		MODE_MUlLIPLY,
	};
}

template<typename T>
struct MaxMin
{
	T min;
	T max;
};
// エミッター構造体
struct Emiter
{
	// ランダム用
	MaxMin<Vector3> renge;     //出現位置 (Vector3の範囲)
	MaxMin<Vector4> color;     // 色 (Vector3の範囲)
	MaxMin<Vector3> size;        // 大きさ (floatの範囲)
	MaxMin<Vector3> rotate;      // 回転 (floatの範囲)
	MaxMin<Vector3> rotateVelocity;// 回転 (floatの範囲)
	MaxMin<float> lifeTime;    // 生存時間 (floatの範囲)
	MaxMin<Vector3> velocity;  // 速度 (Vector3の範囲)

	CornerSegment corner;
	float sphereRad;
	std::vector<Vector3> controlPoints; // 各ポジション

	WorldTransform worldtransform;
	bool isEmit = false;



	int count;
};

// パーティクルグループ
struct ParticleGroup
{
	std::string name;										// 名前
	std::unique_ptr<Material> material = nullptr;			// マテリアルデータ
	std::list<Particle> particle;							// パーティクル
	bool flag;												//　
	StructuredBuffer<ParticleForGPU> sbParticleResource_;	// パーティクルリソース(sBuffer)
	uint32_t instanceCount;									// インスタンス数
	ModelMesh* mesh;										// メッシュ
	Emiter emiter;											// エミッター
	bool usebillboard = true;								// ビルボードするか
	bool isAlpha = false;									// 透明にしていくか
	bool isLine = true;										// ライン描画するか
	bool isGravity = false;									// 重力を有効にするか
	bool isLifeTimeScale_ = false;							// ちっちゃくしていくか
	bool isRotateVelocity = false;							// 回転するか
	bool isBounce = false;									// 跳ねるか
	ParticleData::EmitType emitType = ParticleData::EmitType::kRandom;		// エミッターでの出方
	ParticleData::TopBottom topBottom = ParticleData::TopBottom::kBottom;	// 
	ParticleData::RasterizerType rasteType;									// ラスタライザタイプ
	ParticleData::BlendType blendType;										// ブレンドタイプ
	Transform uvTransformVeloctiy_{ {},{},{0,0,0} };		// uvトランスフォーム
};