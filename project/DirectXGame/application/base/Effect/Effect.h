#pragma once
#include <DirectXGame/engine/Effect/EffectComponent.h>

// 前方宣言
class Entity3DManager;
class Entity2DManager;

/// <summary>
/// エフェクトクラス
/// </summary>
class Effect {
public:
	// 初期化
	void Initialize(Entity3DManager* entity3DManager, GlobalVariables* globalVariables);
	// 更新
	void Update();
	// 出現
	void Emit(const std::string& name, const Vector3& pos);

private:
	// パーティクル初期化
	void InitParticle();
	// パーティクル初期化(範囲攻撃弾)
	void InitRangeBombingBullet();

private:
	std::unique_ptr<EffectComponent> effectComponent_;	// エフェクトコンポーネント

private:
	Entity3DManager* entity3DManager_ = nullptr;	// 3Dエンティティマネージャー
	Entity2DManager* entity2DManager_ = nullptr;	// 2Dエンティティマネージャー
	GlobalVariables* globalVariables_ = nullptr;	// グローバル変数

};
