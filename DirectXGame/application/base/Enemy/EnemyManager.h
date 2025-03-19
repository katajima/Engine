#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object3d.h"
#include"DirectXGame/engine/base/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/struct/Matrix4x4.h"


class BaseEnemy;
class FollowCamera;
class Player;

class EnemyManager
{
public:
	enum EnemyType {
		kMoveTarget,// 動く的
		kTarget,	// 的
		kHardTarget,// 固い的
		kShotEnemy, // 弾撃ち敵
	};


	void Initialize(Camera* camera,FollowCamera* followCamera,Player* player);

	void Update();

	void Draw();

	
	// 動く的
	void AddMoveTarget(float hp, Vector3 pos);
	// 的
	void AddTarget(float hp, Vector3 pos);
	// 固い的
	void AddHardTarget(float hp, Vector3 pos);
	// 撃つ敵
	void AddShotEnemy(float hp, Vector3 pos);

private:
	std::list<std::unique_ptr<BaseEnemy>> baseEnemy_;

	Camera* camera_ = nullptr;
	FollowCamera* followCamera_ = nullptr;
	Player* player_ = nullptr;
};

