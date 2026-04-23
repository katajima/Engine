#pragma once
#include "DirectXGame/engine/Camera/CameraData.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/application/base/Camera/Base/CameraFollow.h"
#include "DirectXGame/application/base/Camera/Base/CameraLag.h"
#include "DirectXGame/application/base/Camera/Base/CameraLockOn.h"
#include "DirectXGame/application/base/Camera/Base/CameraRotation.h"
#include "DirectXGame/application/base/Camera/Base/CameraShake.h"


// カメラ統括クラス
class CameraController {
public:

	// 初期化
	void Initialize(Engine::Camera* camera,const InputSystem* input);

	// 更新
	void Update(float dt);

	// ロックオンターゲット設定
	void SetLockOnTraget(const Engine::WorldTransform* target) { this->lockOnTarget = target; };
	// ターゲット設定
	void SetTraget(const Engine::WorldTransform* target) { this->target = target; };


private:
	// 追従
	std::unique_ptr<CameraFollow> follow = nullptr;
	// 回転
	std::unique_ptr<CameraRotation> rotation = nullptr;
	// ロックオン
	std::unique_ptr<CameraLockOn> lockOn = nullptr;
private:
	const InputSystem* input = nullptr;
	Engine::Camera* camera = nullptr;
	const Engine::WorldTransform* target = nullptr;
	const Engine::WorldTransform* lockOnTarget = nullptr;
};