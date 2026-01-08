#include "BaseCamera.h"

#include "DirectXGame/engine/MyGame/MyGame.h"


/// <summary>
/// オブジェクトの時間取得
/// </summary>
/// <returns></returns>
float BaseCamera::GetTime() const { return Engine::MyGame::GameTime() * timeSpeed_; }

