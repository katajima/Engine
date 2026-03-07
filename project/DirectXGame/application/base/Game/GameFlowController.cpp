#include "GameFlowController.h"
#include <DirectXGame/engine/scene/SceneManager.h>

void GameFlowController::Initialize(Engine::SceneManager* sceneManager, Engine::GlobalVariables* globalVariables, 
	Character::CharacterManager* characterManager){
	this->globalVariables = globalVariables;	// 保存項目
	this->characterManager = characterManager;	// キャラクター管理するクラス
	this->sceneManager = sceneManager;			// シーンマネージャー
	entityManager = sceneManager->GetEntityManager();	// エンティティ3d
	input  = sceneManager->GetInput();						// 入力

	// ゲームイベント制御クラス初期化
	gameEventController_ = std::make_unique<Game::GameEventController>();
	gameEventController_->Initialize(entityManager,globalVariables, characterManager, input);

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

void GameFlowController::SceneChange(){
	if (gameEventController_->IsEndEvent()) {
		sceneManager->ChangeScene("RESULT");
	}
}

