#pragma once
#include "vector"
#include <map>
#include <string>

#include <DirectXGame/engine/Effect/Particle/Emit/ParticleEmitter.h>
#include <DirectXGame/engine/Effect/Particle/CPU/ParticleManager.h>
#include <DirectXGame/engine/Effect/Particle/GPU/GpuParticleManager.h>

#include <DirectXGame/engine/Effect/Trail/TrailEffect.h>
#include"DirectXGame/engine/Effect/Primitive/Primitive.h"
#include "DirectXGame/engine/2d/Primitive2D.h"

class DirectXCommon;
class Entity3DManager;
class GlobalVariables;
class PrimitiveCommon;
class EffectComponent
{
public:
	EffectComponent() = default;
	~EffectComponent() = default;

	void Init(Entity3DManager* entity3dManager, GlobalVariables* globalVariables);
	void SetCamera(Camera* camera) { camera_ = camera; }
	void Update();

	void Draw();
	void DrawEffect();

	GpuParticleManager* GetGpuParticleManager() { return gpuParticleManager_; }

public: // パーティクルエミッター(CPU)

	// 追加
	void AddEmitter(const std::string& name, const std::string& particleName, EmitterShapeType type, EmitData::EmitType emittype = EmitData::EmitType::kRandom , WorldTransform* parent = nullptr);
	// 削除
	void RemoveEmitter(const std::string& name) { emitters_.erase(name); };
	// 削除
	void RemoveEmitterAll() { emitters_.clear(); }
	// 存在確認
	bool HasEmitter(const std::string& name) const {return emitters_.find(name) != emitters_.end();}
	// 取得
	template<typename T>
	T* GetEmitterAs(const std::string& name) {
		auto it = emitters_.find(name);
		if (it != emitters_.end()) {
			return dynamic_cast<T*>(it->second.get());
		}
		return nullptr;
	}

	BaseParticleEmitter* GetBaseEmitter(const std::string& name) {
		auto it = emitters_.find(name);
		if (it != emitters_.end()) {
			return it->second.get();
		}
		return nullptr;
	}

public: // パーティクルエミッター(GPU)
	
	// 追加
	void AddGPUParticleEmitter(const std::string& name, const std::string& particleName, EmitterType type, WorldTransform* parent = nullptr);
	// 取得
	template<typename T>
	T* GetGpuEmitter(const std::string& name) {
		return gpuParticleManager_->GetGpuParticleEmitter(name);
	}


public: // トレイルエフェクト
	// 追加
	void AddTrailEffect(const std::string name,const std::string tex, float maxTime,WorldTransform& parent ,Color color = { 1,1,1,1 }, Vector3 offsetStr = { 0,0.5f,0 }, Vector3 offsetEnd = { 0,-0.5f,0 });
	// 削除
	void RemoveTrailEffect(const std::string& name) { trails_.erase(name); };
	// 削除
	void RemoveTrailEffectAll() { trails_.clear(); };
	// 存在確認
	bool HasTrailEffect(const std::string& name) const { return trails_.find(name) != trails_.end(); }
	// 取得
	TrailEffect* GetTrailEffect(const std::string& name) {
		auto it = trails_.find(name);
		if (it != trails_.end()) {
			return it->second.get();
		}
		return nullptr;
	}
public: // プリミティブ

	// 追加
	void AddPrimitive(const std::string& name,const std::string tex ,ShapeParameter::ShapeType type);	
	// 削除
	void RemovePrimitive(const std::string& name,ShapeParameter::ShapeType type);
	// 削除
	void RemovePrimitiveAll() { primitives3D_.clear(); };
	// 取得
	template<typename T> 
	T* GetPrimitive(const std::string& name, ShapeParameter::ShapeType type) {
		// typeが存在するかチェック
		auto typeIt = primitives3D_.find(type);
		if (typeIt == primitives3D_.end()) {
			return nullptr;
		}

		// nameが存在するかチェック
		auto& nameMap = typeIt->second;
		auto nameIt = nameMap.find(name);
		if (nameIt == nameMap.end()) {
			return nullptr;
		}

		// dynamic_castで安全にキャストして返す
		return dynamic_cast<T*>(nameIt->second.get());
	}



private:
	// パーティクルエミッター群
	std::map<std::string, std::unique_ptr<BaseParticleEmitter>> emitters_;

	// トレイルエフェクト群
	std::map<std::string, std::unique_ptr<TrailEffect>> trails_;

	// プリミティブ群
	std::map <ShapeParameter::ShapeType,std::map<std::string, std::unique_ptr<BasePrimitive>>> primitives3D_;


private:
	Entity3DManager* entity3dManager_ = nullptr;
	ParticleManager* particleManager_ = nullptr;
	GlobalVariables* globalVariables_ = nullptr;
	PrimitiveCommon* primitiveCommon_ = nullptr;
	GpuParticleManager* gpuParticleManager_ = nullptr;
	Camera* camera_ = nullptr;
};

