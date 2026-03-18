#include "playerWeapon.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"

void PlayerWeapon::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera)
{
	this->inputSystem = inputSystem;						// インプット
	this->entityManager = entityManager; // エンティティ3d
	
	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	objectComponent_->Initialize(entityManager, globalVariables, "PlayerWeapon", "Sword.obj", false, false, this);
	objectComponent_->SetSRT(provisionalData_.size, {}, position);	// SRT設定

	// トレイルエフェクト設定
	GetObject3D()->UseTrailEffect("resources/Texture/Image.png", provisionalData_.trailLifeTime, provisionalData_.color,
		GetObject3D()->GetModel()->GetModelData().mesh[0]->GetMin(),
		GetObject3D()->GetModel()->GetModelData().mesh[0]->GetMax());
	// 描画する
	GetObject3D()->SetIsDraw(true);
	// スケール
	GetObject3D()->GetWorldTransform().scale_ = provisionalData_.scale_;
}

void PlayerWeapon::Update(){
	// ワールドトランスフォーム更新
	GetObject3D()->UpdateWorldTransform();
}

void PlayerWeapon::Draw2D(){}

void PlayerWeapon::DrawEffect(){}

