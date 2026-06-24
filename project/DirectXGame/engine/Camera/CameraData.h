#pragma once
#include "DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFunctions.h"
#include "DirectXGame/engine/Collider/CollisionTypeIdDef.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"

// 前方宣言
class InputSystem;

// 追従カメラデータ
struct CameraFollowData {
	// プレイヤーからの相対位置
	Vector3 offset{};
	// 追従ターゲットに加えるワールド座標オフセット
	Vector3 targetOffset{};
	// 追従補間速度
	float followSpeed = 0.0f;
	// スムーズ追従
	bool useSmooth = false;
};

// カメラ回転制御データ
struct CameraRotationData {
	// 左右回転角度
	float yaw = 0.0f;
	// 上下回転角度
	float pitch = 0.0f;
	// 入力感度
	float sensitivity = 0.03f;
	// 最小ピッチ
	float minPitch = -0.8f;
	// 最大ピッチ
	float maxPitch = 0.8f;
};

// カメラロックオンデータ
struct CameraLockOnData {
	// ターゲット位置
	const Engine::WorldTransform* target = nullptr;
	// ロックオン時の回転補間速度
	float rotateSpeed = 0.5f;
	// ロックオンしているか
	bool isLockOn = false;
	// ロックオン後の回転を解除されても引き継ぐか
	bool isLockOnRotate = true;
};

// カメラシェイクデータ
struct CameraShakeData {
	// 揺れの継続時間
	float duration = 0.0f;
	// 揺れオフセット
	Vector3 offset{};
};

// カメラズームデータ
struct CameraZoomData {
	// 目標距離
	float targetDistance = 0.0f;
	// 距離補間速度
	float zoomSpeed = 0.0f;
	// 持続時間
	float duration = 0.0f;
};

// 障害物回避データ
struct CameraCollisionData {
	// カメラ衝突判定半径
	float radius = 1.0f;
	// 衝突対象レイヤー
	CollisionLayer collisionMask;
};

// カメラ遅延
struct CameraLagData {
	// 遅延を有効にするか
	bool enable = false;
	// 位置遅延の補間速度
	float positionLagSpeed = 10.0f;
	// 回転遅延の補間速度
	float rotationLagSpeed = 10.0f;
};

// カメラ先読みデータ
struct CameraLookAheadData {
	// 先読みを有効にするか
	bool enable = false;
	// 最大先読み距離
	float distance = 0.0f;
	// 先読みを始めるターゲット速度
	float minSpeed = 0.0f;
	// 最大先読み距離になるターゲット速度
	float maxSpeed = 30.0f;
	// 先読み位置の補間速度
	float smoothSpeed = 8.0f;
};

// 速度に応じた追従距離データ
struct CameraSpeedZoomData {
	// 速度ズームを有効にするか
	bool enable = false;
	// ズームを始めるターゲット速度
	float minSpeed = 0.0f;
	// 最大ズームになるターゲット速度
	float maxSpeed = 30.0f;
	// 低速時に足すカメラ距離
	float nearOffsetZ = 0.0f;
	// 高速時に足すカメラ距離
	float farOffsetZ = -8.0f;
	// 距離変化の補間速度
	float smoothSpeed = 8.0f;
};

// 攻撃演出などで一時的に注視点をずらすデータ
struct CameraActionOffsetData {
	// 追従ターゲットへ加えるワールド座標オフセット
	Vector3 targetOffset{};
	// オフセットの補間速度
	float blendSpeed = 8.0f;
	// オフセットを維持する時間
	float duration = 0.0f;
};

