#pragma once
#include "DirectXGame/engine/Camera/BaseCamera.h"

/// <summary>
/// StageCameraを管理・実装するクラス。
/// </summary>
class StageCamera : public BaseCamera {
public:
	///< summary>
	/// 初期化
	///</summary>
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position) override;

	///< summary>
	/// 更新
	///</summary>
	void Update() override;


private:
	Vector3 anchorPoint{};
	Vector3 offsetPos{};
};