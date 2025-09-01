#pragma once
#include "DirectXGame/engine/DirectX/Resource/StructuredBuffer.h"
#include "DirectXGame/engine/3d/Model/Model.h"
#include "DirectXGame/engine/Effect/Particle/Emit/EmitData.h"


class Material;


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






// エミッター構造体
struct Emiter
{
	// ランダム用
	Range <Vector3> renge;			//出現位置 (Vector3の範囲)
	Range <Vector4> color;			// 色 (Vector3の範囲)
	Range <Vector3> size;			// 大きさ (floatの範囲)
	Range <Vector3> rotate;			// 回転 (floatの範囲)
	Range <Vector3> rotateVelocity;	// 回転 (floatの範囲)
	Range <float> lifeTime;			// 生存時間 (floatの範囲)
	Range <Vector3> velocity;		// 速度 (Vector3の範囲)
	Range <Vector3> acceleration;	// 加速度 (Vector3の範囲)

	CornerSegment corner;
	float sphereRad;
	std::vector<Vector3> controlPoints; // 各ポジション
	int count = 1;					// エミット数
	
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
	EmitData::IsFlag isFlag;											// 各フラグ
	EmitData::EmitType emitType = EmitData::EmitType::kRandom;		// エミッターでの出方
	EmitData::TopBottom topBottom = EmitData::TopBottom::kBottom;	// 
	EmitData::RasterizerType rasteType;									// ラスタライザタイプ
	EmitData::BlendType blendType;										// ブレンドタイプ
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
		DirectXCommon* dxCommon, ModelMesh* mesh , EmitData::RasterizerType rasteType, EmitData::BlendType blendType);

};