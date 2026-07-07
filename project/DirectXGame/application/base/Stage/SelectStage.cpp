#include "SelectStage.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Camera/Base/CameraManeger.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"

void SelectStage::Initialize(Engine::EntityManager* entityManager, CameraManager* cameraManager) {
	this->entityManager = entityManager;	// エンティティ3d
	this->camera = cameraManager->GetCamera();	// カメラ取得
	// 空初期化
	InitializeSky(entityManager, cameraManager);
	// ライト初期化
	InitializeLight(entityManager);
	
	// 基底クラスの共通処理で地面を生成する。
	InitializeGround(entityManager);

	// セレクト用の向きでプレイヤー車を生成する。
	InitializePlayerCar(entityManager, playerCar_, playerCarPos_, { 0, 0, 0 });

	for (int i = 0; i < 300; i++) {
		std::unique_ptr<ObjectComponent> stone = std::make_unique<ObjectComponent>();
		stone->InitializeInstancing(entityManager,
			{}, "Stone" + std::to_string(i), "Stone.obj", "", false, false, nullptr,
			Engine::ObjectInstans::TransparencyType::kNo,false);
		stone->GetWorldTransform().translate_ = { Random::RandomFloat(-6000, -400.0f) , -20.0f, Random::RandomFloat(-100, 7000.0f) };
		stone->GetWorldTransform().rotate_.y = Random::RandomFloat(0, 360.0f);
		stone->GetWorldTransform().scale_ = Random::RandomFloat(0.5f, 2.25f);
		stoneComponents_.push_back(std::move(stone));
	}
}

void SelectStage::Update(float dt) {
	BaseUpdate(dt);


	tail_->GetMaterial(0)->GetMaterialInstance().transform.translate.y += 0.05f * dt;
	if (tail_->GetMaterial(0)->GetMaterialInstance().transform.translate.y >= 1.0f) {
		tail_->GetMaterial(0)->GetMaterialInstance().transform.translate.y = 0.0f;
	}

	// プレイヤー車更新
	playerCar_->Update(dt);


	for(auto& stone : stoneComponents_){
		stone->GetWorldTransform().translate_.z += 20.0f;
		if (stone->GetWorldTransform().translate_.z >= 7000.0f) {
			
			stone->GetWorldTransform().translate_.x = Random::RandomFloat(-6000, -400.0f);

			stone->GetWorldTransform().translate_.z = 0;
			stone->GetWorldTransform().rotate_.y = Random::RandomFloat(0, 360.0f);
			stone->GetWorldTransform().scale_ = Random::RandomFloat(0.5f, 2.25f);

		}
		stone->Update();
	}
}
