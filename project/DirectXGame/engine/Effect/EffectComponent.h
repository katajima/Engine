#pragma once
#include "vector"
#include <map>
#include <string>

#include <DirectXGame/engine/Effect/Particle/Emit/ParticleEmitter.h>
#include <DirectXGame/engine/Effect/Particle/ParticleManager.h>
#include <DirectXGame/engine/Effect/Trail/TrailEffect.h>


class DirectXCommon;
class Entity3DManager;

class EffectComponent
{
public:
	void Init(Entity3DManager* entity3dManager);
	void SetCamera(Camera* camera) { camera_ = camera; }
	void Update();

	void Draw();
public: // パーティクルエミッター

	// 追加
	void AddEmitter(const std::string& name, const std::string& particleName, EmitterShapeType type, EmitData::EmitType emittype = EmitData::EmitType::kRandom , WorldTransform* parent = nullptr);
	// 削除
	void RemoveEmitter(const std::string& name) { emitters_.erase(name); };
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

public: // トレイルエフェクト
	// 追加
	void AddTrailEffect(const std::string name,const std::string tex, float maxTime,WorldTransform* parent ,Color color = { 1,1,1,1 }, Vector3 offsetStr = { 0,0.5f,0 }, Vector3 offsetEnd = { 0,-0.5f,0 });
	// 削除
	void RemoveTrailEffect(const std::string& name) { trails_.erase(name); };
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

private:
	// パーティクルエミッター群
	std::map<std::string, std::unique_ptr<BaseParticleEmitter>> emitters_;

	// トレイルエフェクト群
	std::map<std::string, std::unique_ptr<TrailEffect>> trails_;


private:
	Entity3DManager* entity3dManager_ = nullptr;
	ParticleManager* particleManager_ = nullptr;
	Camera* camera_ = nullptr;
};

