#include"BaseObject.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

float BaseObject::GetTime() const { return MyGame::GameTime(); }