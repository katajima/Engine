#include "BaseBullet.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"

BaseBullet::~BaseBullet() {
	object_->IsDelete();
}

float BaseBullet::GetTimer() const
{
	return  MyGame::GameTime();
}

void BaseBullet::SetPlayer(BasePlayer* player)
{
	player_ = player;
}

void BaseBullet::SetEnemy(BaseEnemy* enemy)
{
	enemy_ = enemy;
}

// コライダコンポーネント取得
ColliderComponent* BaseBullet::GetColliderComponent() { return object_->GetColliderComponent(); }
