#pragma once
#include "DirectXGame/engine/Camera/BaseCamera.h"

/// <summary>
/// フォローカメラ
/// </summary>
class FollowCamera : public BaseCamera
{
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
public:
	// 追従対象を設定する。
	void SetTarget(Engine::WorldTransform* target) { target_ = target; }

protected:
	// 追従対象への非所有ポインター。
	Engine::WorldTransform* target_ = nullptr;

private: // 一旦

	struct ProvisionalData {
		Vector3 rotate = { Math::DegreesToRadians(20), 0.0f, 0.0f };
		float farClip_ = 15000.0f;

		int gaussianNum = 9;
		float gaussianSigma = 100.0f;
		float bloomIndensity = 10.0f;
	};
	ProvisionalData provisionalData_;

};

