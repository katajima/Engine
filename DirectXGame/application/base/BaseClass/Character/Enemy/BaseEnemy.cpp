#include "BaseEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/Player/Normal/Player.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"


float BaseEnemy::Timer() const
{
	return MyGame::GameTime() * timeSpeed_;
}

void BaseEnemy::Shake()
{

	//shakePos_.x = oldPos_.x + float(rand() % 20 - 10);
	//float xShake = float(shakePos_.x) / static_cast<float>(10);
	//shakePos_.z = oldPos_.z + float(rand() % 20 - 10);
	//float zShake = float(shakePos_.z) / static_cast<float>(10);
	//
	//objectBase_->worldtransform_.translate_.x = xShake;
	//objectBase_->worldtransform_.translate_.z = zShake;
}

void BaseEnemy::HitStpoTime()
{
	bool is = false;
	hitStopTimer -= MyGame::GameTime();
	if (hitStopTimer <= 0.0f) {
		hitStopTimer = 0.0f;
	}
	if (hitStopTimer > 0) {
		is = true;
	}

	if (is) {
		timeSpeed_ = 0.0f;
		Shake();
	}
	else {
		timeSpeed_ = 1.0f;
	}
}

void BaseEnemy::HitMotion()
{
	// ロックオン座標
	Vector3 lockOnPosition = player_->GetObject3D()->GetWorldPosition();

	// 追跡対象からロックオン対象へのベクトル
	Vector3 sub = Subtract(lockOnPosition, GetWorldTransform().translate_);
	Vector3 move = sub.Normalize() * (-hitKnockbackPower);
	
	if (move.y <= hitKonckbackYPower) {
		move.y = hitKonckbackYPower;
	}
	Velocity() = move;
}

void BaseEnemy::Initialize2D()
{
	icon_lockOn = std::make_unique<Sprite>();
	icon_lockOn->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/icon/LockOnW.png");
	icon_lockOn->SetSize(0.10f);
	icon_lockOn->SetColor({ 1,0,1,1 });
	icon_lockOn->SetPosition({ -100,650 });
	icon_lockOn->SetAnchorPoint({ 0.5f,0.5f });

	hpBer_ = std::make_unique<Sprite>();
	hpBer_->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/Image.png");
	//hpBer_->SetSize(0.10f);
	hpBer_->SetColor({ 1,0,0,0.7f });
	hpBer_->SetPosition({ -100,650 });
	hpBer_->SetAnchorPoint({ 0.5f,0.0f });

	backHpBer_ = std::make_unique<Sprite>();
	backHpBer_->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/Image.png");
	//backHpBer_->SetSize(0.10f);
	backHpBer_->SetColor({ 0.1f,0.1f,0.1f,0.7f });
	backHpBer_->SetPosition({ -100,650 });
	backHpBer_->SetAnchorPoint({ 0.5f,0.0f });
}
