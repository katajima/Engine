#pragma once
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Character/Vision/VisionComponent.h"
#include"DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include"DirectXGame/application/base/Special/Base/BaseSpecial.h"

//前方宣言
class BasePlayer;
class Entity3DManager;
class Entity2DManager;

/// <summary>
/// 敵クラス
/// </summary>
class BaseEnemy : public BaseCharacter {
public:
	// 初期化
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;


	// 毎フレーム更新
	virtual void Update() = 0;
	// 描画エフェクト
	virtual void DrawEffect() = 0;
	// 描画2d
	virtual void Draw2D() = 0;
	// 移動
	virtual void Move() = 0;
	// ジャンプ
	virtual void Jump() = 0;
	// 攻撃
	virtual void Attack() = 0;
	// プレイヤー設定
	virtual void SetPlayer(BasePlayer* player) = 0;

public:
	// パーティクル発生
	virtual void Emit() = 0;
	// ID設定
	void SetID(uint32_t id) { id_ = id; }
	// 敵タイプ取得
	EnemyType GetType() const { return type_; };
	// 敵タイプ設定
	void SetType(EnemyType type) { type_ = type; };

	// 対象の位置(プレイヤー)
	Vector3 GetTargetPos();
	// 対象との距離(プレイヤー)
	float GetTargetDistance();
	// 移動方向
	void DirectionMove(float speed);
	// ロックオンされているか
	bool GetIsLockOn() const { return isLockOn_; }
	//
	void SetIsLockOn(bool isLockOn) { isLockOn_ = isLockOn; }

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
	std::unique_ptr <VisionComponent> visionComponent_;			// 視界
	std::unique_ptr<EffectComponent> effectComponent_ = nullptr;
	bool isLockOn_ = false; // ロックオンされているか

};
