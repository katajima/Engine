#pragma once
#include <string>

// カメラ保存データ
struct GlobalCameraData {
    // 使用カメラ
	std::string cameraName = "no";
	// カメラを変更するか
	bool isChangeCamera = false;
	// 移動補間
	float interpolation = 0.0f;
	// カメラをシェイクするか
	bool isShake = false;
	// シェイク量
	float shakeCameraPower = 0.0f;
	
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
};




