#include "GameFlowController.h"
#include <DirectXGame/engine/scene/SceneManager.h>

void GameFlowController::Initialize(Engine::SceneManager* sceneManager, InputSystem* input, CameraManager* cameraManager, Engine::GlobalVariables* globalVariables,
	Character::CharacterManager* characterManager){
	this->globalVariables = globalVariables;	// 保存項目
	this->characterManager = characterManager;	// キャラクター管理するクラス
	this->sceneManager = sceneManager;			// シーンマネージャー
	this->cameraManager = cameraManager;		// カメラ管理
	entityManager = sceneManager->GetEntityManager();	// エンティティ3d
	input = input;						// 入力

	// ゲームイベント制御クラス初期化
	gameEventController_ = std::make_unique<Game::GameEventController>();
	gameEventController_->Initialize(entityManager,globalVariables, cameraManager, characterManager, input);

}

void GameFlowController::Update(float dt) {
	
	// ゲームのイベント管理クラス更新
	gameEventController_->Update(dt);


	gamePlayData_.flowData_.currentWave = gameEventController_->GetCurrentWave();
	gamePlayData_.flowData_.elapsedTime = gameEventController_->GetCurrentWaveTime();
	// シーン変更処理
	SceneChange();
};

void GameFlowController::Draw() {
	gameEventController_->Draw();
}

void GameFlowController::Draw2D() {
	gameEventController_->Draw2D();
};


void GameFlowController::SceneChange(){
	if (gameEventController_->IsEndEvent()) {
		sceneManager->ChangeScene("RESULT");
	}
}

