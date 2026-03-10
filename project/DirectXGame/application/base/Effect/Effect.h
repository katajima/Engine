#pragma once
#include <DirectXGame/engine/Effect/EffectComponent.h>

// 前方宣言
namespace Engine {
	class EntityManager;
}
/// <summary>
/// エフェクトクラス
/// </summary>
class EffectSystem {
public:
	// 初期化
	void Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables);
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
	// パーティクル初期化(スクラップ)
	void InitScrap();
private:
	std::unique_ptr<Engine::EffectComponent> effectComponent_;	// エフェクトコンポーネント

private:
	Engine::EntityManager* entityManager = nullptr;	// 3Dエンティティマネージャー
	Engine::GlobalVariables* globalVariables = nullptr;	// グローバル変数


private: // 一旦

	struct ProvisionalData {
		Vector3 translate = { 0, 107, 0 };
		Vector3 rotate = { 0.341f, 0.0f, 0.0f };
		float farClip_ = 15000.0f;
	};
	ProvisionalData provisionalData_;

private:
	Vector3 scrapRotateRange_ = Math::DegreesToRadians({ 360,360,360 });	// スクラップのオフセット
	Vector3 scrapPos_{};
	Vector3 scrapSize_{ 0.25f,0.25f,0.25f };
	Vector3 scrapRange_{ 0.5f,0.5f,0.5f };
	Vector4 scrapColor_{ 1,1,1,1};
	Vector3 scrapVelocity_{};
	Vector3 scrapVelocityRange_{ 0.0f,0.0f,0.0f };
	float scrapLifeTime_ = 1.0f;	// スクラップの寿命
};
