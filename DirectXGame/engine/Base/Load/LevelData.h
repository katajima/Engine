#pragma once
#include "DirectXGame/engine/math/MathFanctions.h"
#include "DirectXGame/engine/struct/Structs3D.h"
#include <string>

class LevelData
{
public:

	// オブジェクトデータ
	struct ObjectData
	{
		std::string fileName;
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
		int isCollider;
		int type;
		Vector3 size;
	};
	std::vector<ObjectData> objects;
	// スポーンデータ(プレイヤー)
	struct PlayerSpawnData
	{
		Vector3 position;
		Vector3 rotation;
	};
	std::vector<PlayerSpawnData> players;
	// スポーンデータ(敵)
	struct EnemySpawnData
	{
		std::string fileName;
		Vector3 position;
		Vector3 rotation;
		float timer;
		float crrentTimer;
		int count;
		//int crrentCount;
		int isEnable;
	};
	std::vector<EnemySpawnData> enemys;
	std::vector<int> counts;

	// スポーンデータ(アイテム)
	struct ItemSpawnData
	{
		std::string fileName;
		Vector3 position;
		Vector3 rotation;
	};
	std::vector<ItemSpawnData> items;
	// スポーンデータ(ステージオブジェクト)
	struct StageObjectSpawnData
	{
		std::string fileName;
		Vector3 position;
		Vector3 rotation;
	};
	std::vector<StageObjectSpawnData> stageObjects;


private:

};
