#pragma once
#include "DirectXGame/application/base/Character/BaseCharacter.h"

#include "DirectXGame/application/base/Player/Special/RangeBombingSpecial.h"
#include "DirectXGame/application/base/Player/Special/BulletSpecial.h"
#include"DirectXGame/application/base/Player/Weapon/playerWeapon.h"

class FollowCamera;
class BulletManager;
class BasePlayer : public BaseCharacter
{
public:
	// 初期化
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;

	// 毎フレーム更新
	virtual void Update() = 0;

	virtual void DrawEffect() = 0;

	virtual void Draw2D() = 0;

	// 弾マネージャーの設定
	void SetBulletManager(BulletManager* bulletManager) { bulletManager_ = bulletManager; };
	// フォローカメラの設定
	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }

	RangeBombingSpecial* GetRangeBombingSpecial() { return rangeBombingSpecial_.get(); }

	playerWeapon* GetWeapon() { return weapon_.get(); }
protected:
	// スペシャル攻撃
	std::unique_ptr<BulletSpecial> bulletSpecial_;
	// スペシャル攻撃
	std::unique_ptr<RangeBombingSpecial> rangeBombingSpecial_;
	// 武器
	std::unique_ptr<playerWeapon> weapon_;

	BulletManager* bulletManager_;			// 弾管理
	FollowCamera* followCamera_;			// フォローカメラ
};

