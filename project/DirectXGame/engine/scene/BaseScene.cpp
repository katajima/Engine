#include "BaseScene.h"
#include "SceneManager.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

void Engine::BaseScene::AllUpdate(){ Update();}

void Engine::BaseScene::AllDraw2D(){ Draw2D();}

SceneData& Engine::BaseScene::GetSceneData() { return sceneManager_->GetSceneData(); }

float Engine::BaseScene::GetTime() const { return MyGame::GameTime();}