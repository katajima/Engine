#include "PlayerManager.h"

void PlayerManager::Initialize(Input* input,DirectXCommon* dxCommon,Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera)
{
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	dxCommon_ = dxCommon;
	camera_ = camera;
	input_ = input;
}

void PlayerManager::Update()
{
	if (player_ != nullptr) {

	}
}

void PlayerManager::Draw()
{
	if (player_ != nullptr) {

	}
}

void PlayerManager::DrawEffect()
{
	if (player_ != nullptr) {

	}
}

void PlayerManager::Draw2D()
{
	if (player_ != nullptr) {

	}
}

void PlayerManager::SelectPlayer(Type type, Vector3 position)
{
	player_ = nullptr;
	switch (type)
	{
	case PlayerManager::Type::kNormal:
		player_ = std::make_unique<NormalPlayer>();
		player_->Initialize(input_, dxCommon_,entity3DManager_,entity2DManager_,position,camera_);
		break;
	case PlayerManager::Type::kBullet:
		break;
	case PlayerManager::Type::kAttacker:
		break;
	default:
		break;
	}
}
