#pragma once
#include "DirectXGame/application/base/Camera/Base/BaseFollowCamera.h"

/// <summary>
/// フォローカメラ
/// </summary>
class FollowCamera : public BaseFollowCamera
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

private:
	// 統括
	std::unique_ptr<CameraController> controller = nullptr;

private: // 一旦

	struct ProvisionalData {
		Vector3 translate = { 0, 107, 0 };
		Vector3 rotate = { Math::DegreesToRadians(20), 0.0f, 0.0f };
		float farClip_ = 15000.0f;

		int gaussianNum = 9;
		float gaussianSigma = 100.0f;
		float bloomIndensity = 10.0f;

		float rotateMinX = Math::DegreesToRadians(-10);
		float rotateMaxX = Math::DegreesToRadians(15);


		float maxZOffset = 30.0f;


		Vector3 shackWidth = { 0.1f, 0.3f, 0.1f };
		float shackTime = 0.2f;

		float depthScale = 2.0f;

	};
	ProvisionalData provisionalData_;

};

