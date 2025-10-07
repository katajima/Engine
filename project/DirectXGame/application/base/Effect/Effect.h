#pragma once
#include <DirectXGame/engine/Effect/EffectComponent.h>


class Entity3DManager;
class Entity2DManager;
class Effect {
public:

	void Initialize(Entity3DManager* entity3DManager, GlobalVariables* globalVariables);

	void Update();

	void Emit(const std::string& name, const Vector3& pos);

private:
	void InitParticle();

	void InitRangeBombingBullet();

private:
	std::unique_ptr<EffectComponent> effectComponent_;	// エフェクトコンポーネント

private:
	Entity3DManager* entity3DManager_ = nullptr;	// 3Dエンティティマネージャー
	Entity2DManager* entity2DManager_ = nullptr;	// 2Dエンティティマネージャー
	GlobalVariables* globalVariables_ = nullptr;	// グローバル変数

};
