#include "BaseEnemy.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/Player/Normal/Player.h"
#include "DirectXGame/application/base/Camera/FollowCamera/FollowCamera.h"

Vector3 BaseEnemy::GetTargetPos()
{
	return player_->GetObject3D()->GetWorldPosition(); 
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
