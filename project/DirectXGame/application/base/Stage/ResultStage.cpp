#include "ResultStage.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/application/base/Camera/Base/CameraManager.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"

void ResultStage::Initialize(Engine::EntityManager* entityManager, CameraManager* cameraManager) {
	this->entityManager = entityManager;	// エンティティ3d
	this->camera = cameraManager->GetCamera();	// カメラ取得
	// 空初期化
	InitializeSky(entityManager, cameraManager);
	// ライト初期化
	InitializeLight(entityManager);


	// 基底クラスの共通処理で地面を生成する。
	InitializeGround(entityManager);

	// 換金所
	moneyExchangePlace = entityManager->CreateObject3D("MoneyExchangePlace", Engine::ObjectModelType::kNormal, { 0,0.06f,0 }, camera);
	moneyExchangePlace->SetModel("MoneyExchangePlace.obj");
	moneyExchangePlace->GetWorldTransform().rotate_ = { 0,Math::DegreesToRadians(-90),0 };

	// リザルト用の向きでプレイヤー車を生成する。
	InitializePlayerCar(entityManager, playerCar_, playerCarPos_, { 0, Math::DegreesToRadians(-90), 0 });

	// ステート変更
	playerCar_->GetStateMachine()->ChangeState(CarMainState::ResultCashExchange);
}

void ResultStage::Update(float dt) {
	BaseUpdate(dt);
	// プレイヤー車更新
	playerCar_->Update(dt);
}
