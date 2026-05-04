#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"
#include <map>
#include <string>


struct SceneData {
	int playerID = 0;	// プレイヤーのID(選ばれたキャラを選別するため)
	int stageID = 0;	// ステージのID(選ばれたステージを選別するため)
	bool playerDie = false;
	float score = 0.0f;
};