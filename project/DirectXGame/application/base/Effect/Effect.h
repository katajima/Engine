#pragma once
#include "EffectEditor.h"
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
	// エフェクトの作成
	void CreateEffect(const std::string& name, EmitterShapeType shapeType, const EffectGlobalData& data);

	// パーティクル初期化
	void InitParticle();
	// パーティクル初期化(範囲攻撃弾)
	void InitRangeBombingBullet();
	// パーティクル初期化(弾)
	void InitBullet();
	// パーティクル初期化(スクラップ)
	void InitScrap();
private:
	// エフェクトコンポーネント
	std::unique_ptr<Engine::EffectComponent> effectComponent_ = nullptr;	
	// エフェクトエディタ
	std::unique_ptr<EffectEditor> effectEditor_ = nullptr;
private:
	// エンティティマネージャー
	Engine::EntityManager* entityManager = nullptr;	
	// グローバル変数保存
	Engine::GlobalVariables* globalVariables = nullptr;	
private: // 一旦

	struct ProvisionalData {
		Vector3 translate = { 0, 107, 0 };
		Vector3 rotate = { 0.341f, 0.0f, 0.0f };
		float farClip_ = 15000.0f;
	};
	ProvisionalData provisionalData_;

private:

};
