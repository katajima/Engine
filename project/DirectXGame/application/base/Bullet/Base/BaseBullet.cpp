#include "BaseBullet.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"

BaseBullet::~BaseBullet() {
	// オブジェクトを削除
	object_->IsDelete();
}

float BaseBullet::GetTimer() const
{
	return  Engine::MyGame::GameTime() * timeSpeed_;
}

void BaseBullet::SetPlayer(BasePlayer* player)
{
	player_ = player;	// プレイヤー設定
}

void BaseBullet::SetEnemy(BaseEnemy* enemy)
{
	enemy_ = enemy;	// 敵設定
}

// コライダコンポーネント取得
Engine::ColliderComponent* BaseBullet::GetColliderComponent() { return object_->GetColliderComponent(); }
