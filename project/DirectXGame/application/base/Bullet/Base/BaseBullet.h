#pragma once

#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "BulletData.h"

// 前方宣言
class Effect;
namespace Engine{
	class GlobalVariables;
	class Entity3DManager;
	class Entity2DManager;
	class Primitive;
	class Object3d;
	class Sprite;
	class Camera;
}




/// <summary>
/// 弾の基底クラス
/// </summary>
class BaseBullet : public IHitReceiver{
public:
	// デフォルトコンストラクタ
	BaseBullet() = default;
	virtual ~BaseBullet() = 0;
	
	struct Parameters {
		float HP;			// HP
		float MaxHP;		// HPMAX
		float moveSpeed;	// 移動速度
		float damage;		// ダメージ
	};
	
	// 初期化
	virtual void Initialize(Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) = 0;
	// 毎フレーム更新
	virtual void Update() = 0;
	// 描画3d
	virtual void Draw() = 0;
	// 描画エフェクト
	virtual void DrawP() = 0;
	// 描画2d
	virtual void Draw2D() = 0;
	
	// 消すか
	virtual bool IsExpired() const {
		return !GetAlive() && !GetIsEffectPlay(); // ← 演出含めて完全終了
	}
protected:
	// 基盤の初期化
	void BaseInitialize(Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera,
		const std::string name,const std::string modelName);

public:
	// 持ち主の設定
	void SetOwner(BaseCharacter* owner) { owner_ = owner; };
	// ターゲット設定
	void SetTarget(BaseCharacter* target) { target_ = target; }
	// エフェクト設定
	void SetEffect(Effect* effect) { effect_ = effect; };
	// ターゲットの位置取得
	void SetInfo(const BulletInfo& info) { info_ = info; }
	// 範囲設定
	void SetTargetRange(Vector3 pos, float rad) { targetRange_ = { pos,rad }; };
public:
	// 生存判定
	bool GetAlive() const { return isAlive_; }
	// 演出再生中か
	bool GetIsEffectPlay() const { return isEffectPlay_; }

	// ダメージ
	void AddDamage(float damage) {
		parameter_.HP -= damage;
		if (parameter_.HP <= 0) {
			parameter_.HP = 0;
			isAlive_ = false; // 敵が死亡
		}
	}
	// 時間
	float GetTimer() const;

	// オブジェクト
	Engine::Object3d* GetObject3D() { return object_; }
	// コライダコンポーネント取得
	Engine::ColliderComponent* GetColliderComponent();
	// オブジェクトコンポーネント取得
	ObjectComponent* GetObjectComponent() { return objectComponent_.get(); }

protected:
	// 当たり判定をするか
	void SetIsCollision(bool is) { isCollision = is; }
public:
	
protected:
	std::unique_ptr<ObjectComponent> objectComponent_ = nullptr;

	Engine::Object3d* object_;
	
	// 各パラメータ
	Parameters parameter_{};
	// 移動
	Vector3 velocity_{};
	// デスフラグ
	bool isAlive_ = false;
	// effect
	bool isEffectPlay_ = false;

	// ヒット
	bool Hit = false;
	// 
	bool isCollision = true;

	struct TargetRange {
		Vector3 position; // 位置
		float radius;    // 半径
	} targetRange_;

	// タイムスピード
	float timeSpeed_ = 1.0f;

	float groundHeight_ = -3.0f; // 地面の高さ
protected:
	BaseCharacter* owner_ = nullptr;	// 持ち主
	BaseCharacter* target_ = nullptr;	// 標的
protected:
	Effect* effect_;
	BulletInfo info_;


	Engine::GlobalVariables* globalVariables_ = nullptr;
	Engine::Entity3DManager* entity3DManager_ = nullptr;
	Engine::Entity2DManager* entity2DManager_ = nullptr;

};


