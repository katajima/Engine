#pragma once

// エフェクト関連
#include "DirectXGame/engine/effect/Particle/ParticleManager.h"
#include "DirectXGame/engine/effect/Particle/ParticleEmitter.h"
#include "DirectXGame/engine/effect/Trail/TrailEffect.h"
#include "DirectXGame/engine/3d/Object/Object3d.h"


class DirectXCommon;
class Entity3DManager;
class Entity2DManager;
class PlayerEffect
{
public:
	
	void Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager,  Camera* camera);

	void Update();

	void Draw();

public:
	//　ダッシュ関係 

	void SetDashEmitterParent(WorldTransform& world){ dashEmitter_->SetParent(world); }
	ParticleEmitter* GetDashEmitter() {return dashEmitter_.get();}

	// トレイル関係

	TrailEffect* GetTrailEffect() {return trailEffect_.get();}
	void SetTrailParent(Object3d* object);
	void SetIsTrail(bool is) { isTrail_ = is; }

	// 

private:
	// ダッシュ用エミッター
	std::unique_ptr<ParticleEmitter> dashEmitter_ = nullptr;
	// 剣のトレイルエフェクト
	std::unique_ptr<TrailEffect> trailEffect_;
	bool isTrail_ = false;
	Object3d weaponStr;
	Object3d weaponEnd;

private:
	Entity3DManager* entity3DManager_;
};



