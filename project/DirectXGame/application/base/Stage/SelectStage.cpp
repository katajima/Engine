#include "SelectStage.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Camera/Base/CameraManager.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"

namespace {
	// 背景へ配置する岩の個数。
	constexpr int kStoneCount = 300;
	// 岩を配置するX座標の範囲。
	constexpr float kStoneMinPositionX = -6000.0f;
	constexpr float kStoneMaxPositionX = -400.0f;
	// 岩を配置するZ座標の範囲と再配置位置。
	constexpr float kStoneMinPositionZ = -100.0f;
	constexpr float kStoneMaxPositionZ = 7000.0f;
	constexpr float kStoneResetPositionZ = 0.0f;
	// 岩を地面の下へ配置する高さ。
	constexpr float kStonePositionY = -20.0f;
	// 岩のランダム回転角度の範囲。
	constexpr float kStoneMinRotationDegrees = 0.0f;
	constexpr float kStoneMaxRotationDegrees = 360.0f;
	// 岩のランダム拡大率の範囲。
	constexpr float kStoneMinScale = 0.5f;
	constexpr float kStoneMaxScale = 2.25f;
	// 岩を奥方向へ流す1フレーム当たりの移動量。
	constexpr float kStoneMoveSpeedPerFrame = 20.0f;
	// 地面テクスチャをスクロールする速度と折り返し位置。
	constexpr float kGroundTextureScrollSpeed = 0.05f;
	constexpr float kGroundTextureScrollLimit = 1.0f;
}

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

	for (int i = 0; i < kStoneCount; i++) {
		std::unique_ptr<ObjectComponent> stone = std::make_unique<ObjectComponent>();
		stone->InitializeInstancing(entityManager,
			{}, "Stone" + std::to_string(i), "Stone.obj", "", false, false, nullptr,
			Engine::ObjectInstance::TransparencyType::kNo,false);
		stone->GetWorldTransform().translate_ = { Random::RandomFloat(kStoneMinPositionX, kStoneMaxPositionX),
			kStonePositionY, Random::RandomFloat(kStoneMinPositionZ, kStoneMaxPositionZ) };
		stone->GetWorldTransform().rotate_.y = Random::RandomFloat(kStoneMinRotationDegrees, kStoneMaxRotationDegrees);
		stone->GetWorldTransform().scale_ = Random::RandomFloat(kStoneMinScale, kStoneMaxScale);
		stoneComponents_.push_back(std::move(stone));
	}
}

void SelectStage::Update(float dt) {
	BaseUpdate(dt);


	tail_->GetMaterial(0)->GetMaterialInstance().transform.translate.y += kGroundTextureScrollSpeed * dt;
	if (tail_->GetMaterial(0)->GetMaterialInstance().transform.translate.y >= kGroundTextureScrollLimit) {
		tail_->GetMaterial(0)->GetMaterialInstance().transform.translate.y = 0.0f;
	}

	// プレイヤー車更新
	playerCar_->Update(dt);


	for(auto& stone : stoneComponents_){
		stone->GetWorldTransform().translate_.z += kStoneMoveSpeedPerFrame;
		if (stone->GetWorldTransform().translate_.z >= kStoneMaxPositionZ) {
			
			stone->GetWorldTransform().translate_.x = Random::RandomFloat(kStoneMinPositionX, kStoneMaxPositionX);

			stone->GetWorldTransform().translate_.z = kStoneResetPositionZ;
			stone->GetWorldTransform().rotate_.y = Random::RandomFloat(kStoneMinRotationDegrees, kStoneMaxRotationDegrees);
			stone->GetWorldTransform().scale_ = Random::RandomFloat(kStoneMinScale, kStoneMaxScale);

		}
		stone->Update();
	}
}
