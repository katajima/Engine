#pragma once
#include "DirectXGame/engine/struct/Structs3D.h"
#include <string>

// カメラ保存データ
struct GlobalCameraData {
    // 使用カメラ
	std::string cameraName = "no";
	// カメラを変更するか
	bool isChangeCamera = false;
	// 移動補間
	float interpolation = 0.0f;
	// カメラ切り替え開始時間
	float changeCameraStartTime = 0.0f;
	// カメラをシェイクするか
	bool isShake = false;
	// シェイク量
	float shakeCameraPower = 0.0f;
	// シェイク開始時間
	float shakeStartTime = 0.0f;
	// シェイク継続時間
	float shakeDuration = 0.0f;
	// カメラ基準のシェイク幅
	Vector3 shakeOffset = { 0.0f, 0.0f, 0.0f };

	// 攻撃中に注視点をずらすか
	bool isActionTargetOffset = false;
	// 注視点オフセット開始時間
	float actionTargetOffsetStartTime = 0.0f;
	// 注視点オフセット継続時間
	float actionTargetOffsetDuration = 0.0f;
	// 注視点オフセット補間速度
	float actionTargetOffsetBlendSpeed = 8.0f;
	// 追従ターゲットに加えるワールド座標オフセット
	Vector3 actionTargetOffset = { 0.0f, 0.0f, 0.0f };

	// 攻撃中に速度方向の先読みを使うか
	bool isLookAhead = false;
	// 先読み開始時間
	float lookAheadStartTime = 0.0f;
	// 先読み継続時間
	float lookAheadDuration = 0.0f;
	// 先読み最大距離
	float lookAheadDistance = 0.0f;
	// 先読みを始める速度
	float lookAheadMinSpeed = 0.0f;
	// 最大先読みになる速度
	float lookAheadMaxSpeed = 30.0f;
	// 先読み補間速度
	float lookAheadSmoothSpeed = 8.0f;

	// 攻撃中に速度でカメラ距離を変えるか
	bool isSpeedZoom = false;
	// 速度ズーム開始時間
	float speedZoomStartTime = 0.0f;
	// 速度ズーム継続時間
	float speedZoomDuration = 0.0f;
	// 速度ズームを始める速度
	float speedZoomMinSpeed = 0.0f;
	// 最大速度ズームになる速度
	float speedZoomMaxSpeed = 30.0f;
	// 低速時に足すカメラ距離
	float speedZoomNearOffsetZ = 0.0f;
	// 高速時に足すカメラ距離
	float speedZoomFarOffsetZ = -8.0f;
	// 速度ズーム補間速度
	float speedZoomSmoothSpeed = 8.0f;
	
	// カメラをズームするか
	bool isZoom = false;
	// ロックオンされていたらズームするか
	bool isLockOnZoom = false;
	// カメラズーム量
	float zoomTargetDistance = 0.0f;
	// カメラズーム開始時間
	float zoomStartTime = 0.0f;
	// カメラズーム時間
	float zoomDuration = 0.0f;
	// カメラズーム距離補間速度
	float zoomSpeed = 0.0f;

	// ロックオンするか
	bool isLockOn = false;
	// ロックオン後の回転を解除されても引き継ぐか
	bool isLockOnRotate = true;
	// ロックオン補間速度
	float lockOnInterpolation = 0.5f;
	// ロックオン解除時間(0以下なら解除しない)
	float lockOnEndTime = 0.0f;

	// 攻撃がヒットした瞬間に専用のカメラ演出を再生するか
	bool isHitCameraEffect = false;
	// ヒット時にカメラをシェイクするか
	bool isHitShake = true;
	// ヒット時シェイクの継続時間
	float hitShakeDuration = 0.1f;
	// ヒット時のカメラ基準シェイク幅
	Vector3 hitShakeOffset = { 0.2f, 0.2f, 0.2f };
	// ヒット時にカメラをズームするか
	bool isHitZoom = false;
	// ヒット時ズームの距離
	float hitZoomTargetDistance = 0.0f;
	// ヒット時ズームの補間速度
	float hitZoomSpeed = 8.0f;
	// ヒット時ズームの継続時間
	float hitZoomDuration = 0.1f;
};




