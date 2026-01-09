#pragma once
#include "DirectXGame/engine/math/MathFunctions.h"
#include "DirectXGame/engine/struct/Structs3D.h"
#include <string>
#include "DirectXGame/engine/Light/Lights.h"
#include "DirectXGame/engine/Base/pch.h"

/// <summary>
/// レベルデータのクラス
/// </summary>
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
	//　カメラデータ
	struct CameraObject
	{
		std::string fileName;
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
	};
	std::vector<CameraObject> cameras;
	//
	std::vector <DirectionalLightData> directionalDatas;
	std::vector <PointLightData> pointDatas;
	std::vector <SpotLightData> spotDatas;

private:

};
