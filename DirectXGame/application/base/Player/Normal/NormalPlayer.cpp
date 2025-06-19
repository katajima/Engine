#include "NormalPlayer.h"

void NormalPlayer::Initialize(Input* input,DirectXCommon* dxcommon, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Vector3 position, Camera* camera)
{
	//Collider::Initialize(camera);
	//Collider::SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayer));

	input_ = input;
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	camera_ = camera;
	dxCommon_ = dxcommon;

	object_ = std::make_unique<Object3d>();
	object_->Initialize(entity3DManager);
	object_->SetCamera(camera_);
	object_->worldtransform_.translate_ = position;
	object_->Update();

	// レティクル
	objectReticle_.Initialize(entity3DManager);
	objectReticle_.SetCamera(camera_);
	objectReticle_.SetModel("enemy.obj");
	objectReticle_.worldtransform_.parent_ = &object_->worldtransform_;
	objectReticle_.worldtransform_.translate_ = { 0,0,30 };

	// 体
	objectBody_.Initialize(entity3DManager);
	objectBody_.SetCamera(camera_);
	objectBody_.SetModel("AnimatedCube.gltf");
	objectBody_.worldtransform_.parent_ = &object_->worldtransform_;
	objectBody_.GetMaterial(0)->shininess_ = 10000;


	// スペシャル攻撃
	bulletSpecial_ = std::make_unique<BulletSpecial>();
	bulletSpecial_->Initialize(entity3DManager, entity2DManager, camera_);
	bulletSpecial_->SetParent(&object_->worldtransform_);



	weapon_ = std::make_unique<playerWeapon>();
	weapon_->Initialize(entity3DManager, camera);
	weapon_->GetObject3D().worldtransform_.parent_ = &object_->worldtransform_;
	weapon_->GetObject3D().worldtransform_.translate_ = { 0,0.5f,0.5f };
	weapon_->SetOffset({ 0,5.0f,0.5f });
	weapon_->SetPlayer(this);




	// UI
	ui_->Initialize(entity2DManager);


	/// エフェクト関係
	effect_->Initialize(dxCommon_, entity3DManager_, entity2DManager, camera_);
	// トレイルエフェクト
	effect_->GetTrailEffect()->SetObject(&weapon_->GetObject3D());
	effect_->SetTrailEffectParent(&weapon_->GetObject3D());
	effect_->SetTrailParent(&weapon_->GetObject3D());
	// ダッシュ用エフェクト
	effect_->SetDashEmitterParent(weapon_->GetObject3D().worldtransform_);

}

void NormalPlayer::Update()
{
}

void NormalPlayer::Draw()
{
}

void NormalPlayer::DrawP()
{
}

void NormalPlayer::Draw2D()
{
}

void NormalPlayer::Move()
{

}
