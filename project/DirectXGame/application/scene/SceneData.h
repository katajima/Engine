#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"
#include <map>
#include <string>


struct SceneData {
	int playerID = 0;	// プレイヤーのID(選ばれたキャラを選別するため)
	int stageID = 0;	// ステージのID(選ばれたステージを選別するため)
};