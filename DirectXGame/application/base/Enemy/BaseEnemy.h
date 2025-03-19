#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object3d.h"
#include"DirectXGame/engine/base/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/struct/Matrix4x4.h"

#include "DirectXGame/engine/collider/3d/Collider.h"
#include"DirectXGame/engine/collider/ContactRecord.h"

class BaseEnemy : public Collider
{
public:
	virtual ~BaseEnemy() = default;

	// 初期化
	virtual void Initialize(Vector3 position, float HP, Camera* camera) = 0;

	// 毎フレーム更新
	virtual void Update() = 0;

	// 描画
	virtual void Draw() = 0;

	//
	virtual void DrawP() = 0;

	virtual void Draw2D() = 0;

	virtual Object3d& GetObject3D() = 0;

private:

public:

	//BaseEnemy() = default;
};

