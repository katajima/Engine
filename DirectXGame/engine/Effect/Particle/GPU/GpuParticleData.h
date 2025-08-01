#pragma once
#include "DirectXGame/engine/Math/MathFanctions.h"

#include "DirectXGame/engine/DirectX/Resource/StructuredBuffer.h"
#include "DirectXGame/engine/DirectX/Resource/ConstantBuffer.h"

struct ParticleCS {
	Vector4 color;
	Vector3 transrate;
	float lifeTime;
	Vector3 scale;
	float currentTime;
	Vector3 velocity;
	float pad[1];
	Vector3 acceleration;
	float pad2[1];
};

struct PreView {
	Matrix4x4 viewProjection;
	Matrix4x4 billboardMatrix;
};

struct PerFrame
{
	// ゲームを起動してからの時間
	float time;
	// 1フレームの経過時間
	float deltaTime;
};

struct EmitterSphere
{
	Vector3 translate;      // 位置
	float radius;           // 射出半径
	Vector3 scale;			// サイズ
	float lifeTime;			// 生存時間
	Vector3 scaleRange;		// サイズ(範囲)
	float lifeTimeRange;	// 生存時間(範囲)
	Vector3 velocity;		// 速度
	uint32_t count;         // 射出数
	Vector3 velocityRange;	// 速度(範囲)
	uint32_t emit;          // 射出許可
	Vector3 color;			// 色
	float frequency;        // 射出間隔
	Vector3 colorRange;		// 色(範囲)
	float frequencyTime;    // 射出間隔調整用時間
};

struct EffectFieldCS {
	Vector3 translate;      // 位置
	float force;			// 力
	Vector3 range;			// 各半径
	uint32_t isEffect;		// 影響を出すか
};

struct MaxInstance
{
	uint32_t maxInstance;	// 最大個数
};

struct FreeListIndex {
	int32_t index;
};

struct ParticleCount
{
	int32_t count;
};

