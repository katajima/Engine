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
	float shakeCameraPower;
	// カメラをズームするか
	bool isZoom = false;
	// カメラズーム量
	float zoomCameraAmount;
	// ロックオンするか
	bool isLockOn = false;
	// ロックオン補間速度
	float lockOnInterpolation = 0.5f;
};




