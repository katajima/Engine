#pragma once
#include "DirectXGame/application/base/BaseClass/Character/BaseCharacter.h"
#include "DirectXGame/application/base/Vision/VisionComponent.h"
#include "DirectXGame/application/base/Hit/HitComponent.h"

class BasePlayer;
class Entity3DManager;
class Entity2DManager;
class BaseEnemy : public BaseCharacter {
public:
	// 初期化
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;


	// 毎フレーム更新
	virtual void Update() = 0;

	virtual void DrawEffect() = 0;

	virtual void Draw2D() = 0;

	virtual void Move() = 0;

	virtual void Jump() = 0;

	virtual void Attack() = 0;

	virtual void SetPlayer(BasePlayer* player) = 0;

public:
	// パーティクル発生
	virtual void Emit() = 0;
	
	void SetID(uint32_t id) { id_ = id; }
	// 敵タイプ取得
	EnemyType GetType() const { return type_; };
	// 敵タイプ設定
	void SetType(EnemyType type) { type_ = type; };

	Vector3 GetTargetPos();


	HitMotionComponent* GetHitMotionComponent() { return hitMotionComponent_.get(); }

	virtual void ChangeState(const std::string& name) = 0;

protected:
	// スプライト初期化
	void Initialize2D();

protected:
	EnemyType type_ = EnemyType::kNormal; // 敵の種類
	uint32_t id_ = 0; // ID
protected: //2D
	std::unique_ptr<Sprite> icon_lockOn;
	std::unique_ptr<Sprite> hpBer_;
	std::unique_ptr<Sprite> backHpBer_;
protected:
	BasePlayer* player_;
	std::unique_ptr <VisionComponent> visionComponent_;
	std::unique_ptr <HitMotionComponent> hitMotionComponent_;
};
