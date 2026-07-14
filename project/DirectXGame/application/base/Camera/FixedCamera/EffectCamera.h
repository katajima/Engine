#pragma once
#include "DirectXGame/application/base/Camera/Base/BaseFixedCamera.h"


/// <summary>
/// 固定カメラ
/// </summary>
class EffectCamera : public BaseFixedCamera
{
public:
	///< summary>
	/// 初期化
	///</summary>
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables, Vector3 position) override;

	///< summary>
	/// 更新
	///</summary>
	void Update() override;
private:



private: // 一旦

	struct ProvisionalData {
		Vector3 translate = { 0, 25, -50 };
		Vector3 rotate = { 0.341f, 0.0f, 0.0f };
		float farClip_ = 15000.0f;
	};
	ProvisionalData provisionalData_;

};

