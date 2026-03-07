#pragma once
#include "DirectXGame/application/base/Camera/Base/BaseFixedCamera.h"

/// <summary>
/// リザルト用カメラ
/// </summary>
class ResultCamera : public BaseFixedCamera {
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

};