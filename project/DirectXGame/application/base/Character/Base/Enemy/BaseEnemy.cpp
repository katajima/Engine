#include "BaseEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayer.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"

Vector3 BaseEnemy::GetTargetPos()
{
	// ターゲット位置
	return player_->GetObjectComponent()->GetWorldTransform().GetWorldPosition();
}

float BaseEnemy::GetTargetDistance()
{
	// ターゲットとの距離
	return GetObjectComponent()->GetWorldTransform().GetWorldPosition().DistanceXZ(player_->GetObjectComponent()->GetWorldTransform().GetWorldPosition());
}

void BaseEnemy::DirectionMoveVelocity(float speed)
{
	// 距離
	Vector3 dire = Subtract(GetTargetPos(), GetWorldTransform().translate_).Normalize();
	
	// 回転設定
	Vector3 rotate = Math::DirectionToRotate(dire, Dire::Z);

	// Y軸周り角度
	GetWorldTransform().rotate_.y = rotate.y;

	// 速度設定
	Velocity() = dire * speed;
}

void BaseEnemy::DirectionMove(float speed)
{
	// 距離設定
	Vector3 dire = Subtract(GetTargetPos(), GetWorldTransform().translate_).Normalize();
	// 回転設定
	Vector3 rotate = Math::DirectionToRotate(dire, Dire::Z);

	// Y軸周り角度
	GetWorldTransform().rotate_.y = rotate.y;
}

Vector3 BaseEnemy::TargetDirection()
{
	// ターゲットの方向
	return Subtract(GetTargetPos(), GetWorldTransform().translate_).Normalize();
}

void BaseEnemy::Initialize2D()
{

	// ロックオン
	icon_lockOn = std::make_unique<Engine::Sprite>();
	icon_lockOn->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/icon/LockOnW.png", false);
	icon_lockOn->SetSize(0.10f);		// サイズ指定
	icon_lockOn->SetColor({ 1,0,1,1 });	// 色指定
	icon_lockOn->SetPosition({ -100,650 });	// 位置指定
	icon_lockOn->SetAnchorPoint({ 0.5f,0.5f });	// アンカーポイント設定
}
