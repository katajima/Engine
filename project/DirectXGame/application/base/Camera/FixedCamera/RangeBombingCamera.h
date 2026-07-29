#pragma once
#include "DirectXGame/application/base/Camera/Base/BaseFixedCamera.h"

/// <summary>
/// 範囲爆撃の着弾地点を真上から映すカメラ。
/// </summary>
class RangeBombingCamera : public BaseFixedCamera
{
public:
	/// <summary>
	/// カメラを初期化する。
	/// </summary>
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position) override;

	/// <summary>
	/// カメラを毎フレーム更新する。
	/// </summary>
	void Update() override;

	/// <summary>
	/// 着弾地点を真上視点の中心へ設定する。
	/// </summary>
	/// <param name="position">着弾地点のワールド座標。</param>
	void SetTargetPosition(const Vector3& position);

private:
	// 真上から見下ろすカメラの高さ。
	float topDownHeight_ = 1200.0f;
	// 現在の着弾地点。
	Vector3 targetPosition_{};
	// 着弾地点が設定済みかを示すフラグ。
	bool hasTargetPosition_ = false;

private:
	struct ProvisionalData {
		// カメラの描画範囲。
		float farClip_ = 15000.0f;
	};
	ProvisionalData provisionalData_;
};