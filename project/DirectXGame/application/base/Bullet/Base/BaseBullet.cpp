#include "BaseBullet.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"

BaseBullet::~BaseBullet() {
	// オブジェクトを削除
	//objectComponent_->IsDelete();
}

void BaseBullet::BaseInitialize(Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, 
	Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera,
	const std::string name, const std::string modelName){
	entity3DManager_ = entity3DManager;	// エンティティ3d
	entity2DManager_ = entity2DManager;	// エンティティ2d
	globalVariables_ = globalVariables;	// 保存項目

	object_ = entity3DManager->CreateObject3D(name, Engine::ObjectModelType::kNormal, position, camera);
	object_->SetModel(modelName);	// モデル設定

	// コンポーネント作成
	//objectComponent_ = std::make_unique<ObjectComponent>();
	//objectComponent_->InitializeInstancing(entity3DManager_, globalVariables_, name, modelName,"",true,false,this);
}

float BaseBullet::GetTimer() const
{
	return  Engine::MyGame::GameTime() * timeSpeed_;
}

// コライダコンポーネント取得
Engine::ColliderComponent* BaseBullet::GetColliderComponent() { return object_->GetColliderComponent(); }
