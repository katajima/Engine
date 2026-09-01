#pragma once
#include "DirectXGame/engine/Camera/BaseCamera.h"

/// <summary>
/// TitleCameraを管理・実装するクラス。
/// </summary>
class TitleCamera : public BaseCamera {
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