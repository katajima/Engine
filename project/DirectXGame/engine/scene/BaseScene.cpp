#include "BaseScene.h"
#include "SceneManager.h"

void BaseScene::AllUpdate()
{
    Update();
}

void BaseScene::AllDraw2D()
{
    Draw2D();
}

SceneData& BaseScene::GetSceneData() { return sceneManager_->GetSceneData(); }