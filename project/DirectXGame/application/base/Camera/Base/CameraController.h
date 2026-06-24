#pragma once
#include "DirectXGame/engine/Camera/CameraData.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/application/base/Camera/Base/CameraFollow.h"
#include "DirectXGame/application/base/Camera/Base/CameraLag.h"
#include "DirectXGame/application/base/Camera/Base/CameraLockOn.h"
#include "DirectXGame/application/base/Camera/Base/CameraRotation.h"
#include "DirectXGame/application/base/Camera/Base/CameraShake.h"
#include "DirectXGame/application/base/Camera/Base/CameraCollision.h"
#include "DirectXGame/application/base/Camera/Base/CameraZoom.h"

class InputSystem;

// カメラ統括クラス
class CameraController {
public:

	// 初期化
	void Initialize(Engine::Camera* camera,const InputSystem* input);

	// 更新
	void Update(float dt);

	// ロックオンターゲット設定
	void SetLockOnTarget(const Engine::WorldTransform* target);
	void SetLockOnTraget(const Engine::WorldTransform* target) { SetLockOnTarget(target); };
	// ターゲット設定
	void SetTraget(const Engine::WorldTransform* target);
	// カメラシェイク取得
	CameraShake* GetShake(){ return shake.get(); }
	// カメラズーム取得
	CameraZoom* GetZoom(){ return zoom.get(); }
	// ロックオン取得
	CameraLockOn* GetCameraLockOn() { return lockOn.get(); }
	// 先読みデータ設定
	void SetLookAheadData(const CameraLookAheadData& data);
	// 速度ズームデータ設定
	void SetSpeedZoomData(const CameraSpeedZoomData& data);
	// 攻撃などの一時先読みリクエスト
	void RequestLookAhead(const CameraLookAheadData& data, float duration);
	// 攻撃などの一時速度ズームリクエスト
	void RequestSpeedZoom(const CameraSpeedZoomData& data, float duration);
	// 攻撃などの一時注視点オフセットリクエスト
	void RequestActionTargetOffset(const CameraActionOffsetData& data);
	// 一時カメラ演出の解除
	void ClearActionAssist();
private:
	// 追従対象の移動量から速度を推定する
	void UpdateTargetMotion(float dt);
	// 先読みと演出オフセットを追従ターゲットへ反映する
	void UpdateFollowAssist(float dt);
	// 速度から追従距離を補間する
	void UpdateSpeedZoom(float dt);
	// 一時リクエストの残り時間を更新する
	void UpdateActionTimers(float dt);
	// 速度に応じた0.0fから1.0fの割合を計算する
	float CalculateSpeedRate(float minSpeed, float maxSpeed) const;
	// 速度方向へどれだけ先読みするか計算する
	Vector3 CalculateLookAheadOffset(const CameraLookAheadData& data) const;
private:
	// 追従
	std::unique_ptr<CameraFollow> follow = nullptr;
	// 回転
	std::unique_ptr<CameraRotation> rotation = nullptr;
	// ロックオン
	std::unique_ptr<CameraLockOn> lockOn = nullptr;
	// 判定
	std::unique_ptr<CameraCollision> collision = nullptr;
	// シェイク
	std::unique_ptr<CameraShake> shake = nullptr;
	// ズーム
	std::unique_ptr<CameraZoom> zoom = nullptr;
private:
	const InputSystem* input = nullptr;
	Engine::Camera* camera = nullptr;
	const Engine::WorldTransform* target = nullptr;
	const Engine::WorldTransform* lockOnTarget = nullptr;
	// 追従対象の前フレーム位置
	Vector3 previousTargetPosition_ = {};
	// 追従対象の推定速度
	Vector3 targetVelocity_ = {};
	// 追従対象の前フレーム位置が有効か
	bool hasPreviousTargetPosition_ = false;
	// 通常時の先読み設定
	CameraLookAheadData lookAheadData_{};
	// 攻撃中などに一時的に使う先読み設定
	CameraLookAheadData actionLookAheadData_{};
	// 通常時の速度ズーム設定
	CameraSpeedZoomData speedZoomData_{};
	// 攻撃中などに一時的に使う速度ズーム設定
	CameraSpeedZoomData actionSpeedZoomData_{};
	// 攻撃中などに一時的に使う注視点オフセット設定
	CameraActionOffsetData actionOffsetData_{};
	// 補間済みの先読みオフセット
	Vector3 currentLookAheadOffset_ = {};
	// 補間済みの注視点オフセット
	Vector3 currentActionTargetOffset_ = {};
	// 補間済みの速度ズーム距離
	float currentSpeedZoomOffsetZ_ = 0.0f;
	// 一時先読みの残り時間
	float actionLookAheadTimer_ = 0.0f;
	// 一時速度ズームの残り時間
	float actionSpeedZoomTimer_ = 0.0f;
	// 一時注視点オフセットの残り時間
	float actionOffsetTimer_ = 0.0f;
	// 一時先読みを使用中か
	bool isActionLookAheadActive_ = false;
	// 一時速度ズームを使用中か
	bool isActionSpeedZoomActive_ = false;
	// 一時注視点オフセットを使用中か
	bool isActionOffsetActive_ = false;
};
