#include "GameFlowController.h"
#include <DirectXGame/engine/scene/SceneManager.h>

void GameFlowController::Initialize(Engine::SceneManager* sceneManager, Engine::GlobalVariables* globalVariables, 
	BaseCharacterManager* characterManager){
	this->globalVariables = globalVariables;	// 保存項目
	this->characterManager = characterManager;	// キャラクター管理するクラス
	this->sceneManager = sceneManager;			// シーンマネージャー
	entity3DManager = sceneManager->GetEntity3DManager();	// エンティティ3d
	input  = sceneManager->GetInput();						// 入力

	// ゲームイベント制御クラス初期化
	gameEventController_ = std::make_unique<GameEventController>();
	gameEventController_->Initialize(entity3DManager,globalVariables, characterManager);

}

void GameFlowController::Update(float dt) {
	
	// ゲームのイベント管理クラス更新
	gameEventController_->Update(dt);

	// シーン変更処理
	SceneChange();
};

void GameFlowController::Draw() {
	gameEventController_->Draw();
}

void GameFlowController::SceneChange(){
	if (isEndgame_) {
		sceneManager->ChangeScene("TITLE");
	}
}

