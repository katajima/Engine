#pragma once
#include <DirectXGame/engine/Effect/EffectComponent.h>

// 前方宣言
namespace Engine {
	class Entity3DManager;
	class Entity2DManager;
}
/// <summary>
/// エフェクトクラス
/// </summary>
class Effect {
public:
	// 初期化
	void Initialize(Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables);
	// 更新
	void Update();
	// 出現
	void Emit(const std::string& name, const Vector3& pos);
	// 出現(方向付き)
	void Emit(const std::string& name, const Vector3& pos, const Vector3& dir, const Vector3& range);

private:
	// パーティクル初期化
	void InitParticle();
	// パーティクル初期化(範囲攻撃弾)
	void InitRangeBombingBullet();
	// パーティクル初期化(弾)
	void InitBullet();

private:
	std::unique_ptr<Engine::EffectComponent> effectComponent_;	// エフェクトコンポーネント

private:
	Engine::Entity3DManager* entity3DManager = nullptr;	// 3Dエンティティマネージャー
	Engine::Entity2DManager* entity2DManager = nullptr;	// 2Dエンティティマネージャー
	Engine::GlobalVariables* globalVariables = nullptr;	// グローバル変数


private: // 一旦

	struct ProvisionalData {
		Vector3 translate = { 0, 107, 0 };
		Vector3 rotate = { 0.341f, 0.0f, 0.0f };
		float farClip_ = 15000.0f;
	};
	ProvisionalData provisionalData_;
};
