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
	Transform transform;			// SRT
	Vector3 velocity;				// 速度
	Vector3 acceleration;			// 加速
	Vector4 color;					// 色
	float lifeTime;					// 生存時間
	float currentTime;				// 経過時間
									   
	Transform strtTransform;		// 初期SRT
	Vector3 rotateVelocity;			// 回転速度
	bool isDestroy = false;			// 破棄フラグ

	bool isEvent = false;			// イベントフラグ

	std::vector<std::string> fieldEffect; // 

};

template<typename T>
struct MaxMin
{
	T min;
	T max;
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


// エミッター構造体
struct Emiter
{
	// ランダム用
	MaxMin<Vector3> renge;			//出現位置 (Vector3の範囲)
	MaxMin<Vector4> color;			// 色 (Vector3の範囲)
	MaxMin<Vector3> size;			// 大きさ (floatの範囲)
	MaxMin<Vector3> rotate;			// 回転 (floatの範囲)
	MaxMin<Vector3> rotateVelocity;	// 回転 (floatの範囲)
	MaxMin<float> lifeTime;			// 生存時間 (floatの範囲)
	MaxMin<Vector3> velocity;		// 速度 (Vector3の範囲)
	MaxMin<Vector3> acceleration;	// 加速度 (Vector3の範囲)

	CornerSegment corner;
	float sphereRad;
	std::vector<Vector3> controlPoints; // 各ポジション
	WorldTransform worldtransform;
	bool isEmit = false;
	int count;

	bool isEvent = false;

	std::vector<std::string> fieldName; // 名前

};

// パーティクルグループ
struct ParticleGroup
{
	std::string name;														// 名前
	std::unique_ptr<Material> material = nullptr;							// マテリアルデータ
	std::list<Particle> particle;											// パーティクル
	

	StructuredBuffer<ParticleForGPU> sbParticleResource_;					// パーティクルリソース(sBuffer)
	uint32_t instanceCount;													// インスタンス数
	ModelMesh* mesh;														// メッシュ
	Emiter emiter;															// エミッター
	ParticleData::IsFlag isFlag;											// 各フラグ
	ParticleData::EmitType emitType = ParticleData::EmitType::kRandom;		// エミッターでの出方
	ParticleData::TopBottom topBottom = ParticleData::TopBottom::kBottom;	// 
	ParticleData::RasterizerType rasteType;									// ラスタライザタイプ
	ParticleData::BlendType blendType;										// ブレンドタイプ
	Transform uvTransformVeloctiy_{ {},{},{0,0,0} };						// uvトランスフォーム
	float kGravitationalAcceleration = 9.8f;								// 重力
};


class DirectXCommon;
namespace ParticleFanction {

	// ビルボード
	Matrix4x4 Billboard(ParticleGroup& group, std::list<Particle>::iterator& particleIterator, Camera* camera);

	// パーティクル影響
	void Effect(ParticleGroup& group, std::list<Particle>::iterator& particleIterator, float deltaTime);

	// パーティクルデータをGPUに送る
	void WorldDataForGPU(ParticleGroup& group, std::list<Particle>::iterator& particleIterator, Camera* camera);

	void MaterialEffect(ParticleGroup& group);

	void Create(ParticleGroup& particleGrou,const std::string name, const std::string textureFilePath , uint32_t kNumMaxInstance,
		DirectXCommon* dxCommon, ModelMesh* mesh , ParticleData::RasterizerType rasteType, ParticleData::BlendType blendType);

};