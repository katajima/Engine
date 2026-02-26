#include "SelectStage.h"

void SelectStage::Initialize(Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
	Engine::GlobalVariables* globalVariables, Engine::Camera* camera) {
	this->entity3DManager = entity3DManager;	// エンティティ3d
	this->entity2DManager = entity2DManager;	// エンティティ2d
	this->globalVariables = globalVariables;	// 保存項目
	this->camera = camera;						// カメラ

	// 車
	playerCar_ = std::make_unique<PlayerCar>();
	playerCar_->Initialize(entity3DManager, globalVariables, {}, { 0,Math::DegreesToRadians(-90),0 });
	playerCar_->SetGroungHeight(0.1f);

}

void SelectStage::Update(float dt) {

}

void SelectStage::Draw() {

}

void SelectStage::DrawP() {

}
