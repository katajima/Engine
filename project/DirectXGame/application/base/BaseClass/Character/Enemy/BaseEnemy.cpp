#include "BaseEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/Player/Normal/Player.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"

Vector3 BaseEnemy::GetTargetPos()
{
	return player_->GetObjectComponent()->GetWorldTransform().GetWorldPosition();
}

float BaseEnemy::GetTargetDistance()
{
	return GetObjectComponent()->GetWorldTransform().GetWorldPosition().DistanceXZ(player_->GetObjectComponent()->GetWorldTransform().GetWorldPosition());
}

void BaseEnemy::DirectionMove(float speed)
{
	Vector3 dire = Subtract(GetTargetPos(), GetWorldTransform().translate_).Normalize();
	Velocity() = { 0,0,0 };
	Vector3 rotate = Math::DirectionToRotate(dire, Dire::Z);

	// Y軸周り角度
	GetWorldTransform().rotate_.y = rotate.y;

	Velocity() = dire * speed;
}

void BaseEnemy::Initialize2D()
{
	icon_lockOn = std::make_unique<Sprite>();
	icon_lockOn->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/icon/LockOnW.png", false);
	icon_lockOn->SetSize(0.10f);
	icon_lockOn->SetColor({ 1,0,1,1 });
	icon_lockOn->SetPosition({ -100,650 });
	icon_lockOn->SetAnchorPoint({ 0.5f,0.5f });

	hpBer_ = std::make_unique<Sprite>();
	hpBer_->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/Image.png" ,false);
	//hpBer_->SetSize(0.10f);
	hpBer_->SetColor({ 1,0,0,0.7f });
	hpBer_->SetPosition({ -100,650 });
	hpBer_->SetAnchorPoint({ 0.5f,0.0f });

	backHpBer_ = std::make_unique<Sprite>();
	backHpBer_->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/Image.png", false);
	//backHpBer_->SetSize(0.10f);
	backHpBer_->SetColor({ 0.1f,0.1f,0.1f,0.7f });
	backHpBer_->SetPosition({ -100,650 });
	backHpBer_->SetAnchorPoint({ 0.5f,0.0f });
}
