#include "BaseBullet.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"
#include <DirectXGame/engine/Manager/Entity/EntityManager.h>
#include "DirectXGame/engine/3d/Model/RenderComponent.h"
BaseBullet::~BaseBullet() {
	// オブジェクトを削除
}

void BaseBullet::BaseInitialize(Engine::EntityManager* entityManager, 
	Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera,
	const std::string name, const std::string modelName){
	this->entityManager = entityManager;	// エンティティ3d
	this->globalVariables = globalVariables;	// 保存項目

	object_ = entityManager->CreateObject3D(name, Engine::ObjectModelType::kNormal, position, camera);
	object_->SetModel(modelName);	// モデル設定
}

float BaseBullet::GetTimer() const
{
	return  Engine::MyGame::GameTime() * timeSpeed_;
}

// コライダコンポーネント取得
Engine::ColliderComponent* BaseBullet::GetColliderComponent() { return object_->GetColliderComponent(); }
