#pragma once
#include "DirectXGame/engine/Camera/BaseCamera.h"
#include "DirectXGame/engine/Animation/AnimationData.h"


/// <summary>
/// ミサイルを映すカメラ
/// </summary>
class UniverseCamera : public BaseCamera
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
	LerpMinMax scaleLerp_;

	float strScaleZCamera = 5.5f;
	float endScaleZCamera = 1.0f;


private:
	// カメラ演出の経過時間を管理するタイマー。
	Timer timer_;

private: // 一旦

	struct ProvisionalData {
		Vector3 translate = { 3200, 107, 3820 };
		Vector3 rotate = { -0.141f, 2.620f, 0.0f };
		float farClip_ = 15000.0f;

		float maxT = 1.25f;
		float scaleZCamera = 2.0f;
	};
	ProvisionalData provisionalData_;

};

