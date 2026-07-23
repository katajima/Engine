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
/// <summary>
/// CameraControllerを管理・実装するクラス。
/// </summary>
class CameraController {
public:

	/// <summary>追従、回転、ロックオン、衝突補正、演出用の各カメラ機能を初期化する。</summary>
	/// <param name="camera">制御対象のカメラ。CameraControllerは所有せず、利用中は有効であること。</param>
	/// <param name="input">カメラ操作に使用する入力。自動制御のみの場合はnullptrを許容する。</param>
	void Initialize(Engine::Camera* camera,const InputSystem* input);

	/// <summary>設定済みの追従対象と入力に基づいて全カメラ機能を更新する。</summary>
	/// <param name="dt">秒単位のフレーム時間。0以下の場合、時間依存の補間は進行しない。</param>
	void Update(float dt);

	/// <summary>カメラが注視するロックオン対象を設定する。</summary>
	/// <param name="target">対象への非所有ポインター。ロックオン解除時はnullptr。</param>
	void SetLockOnTarget(const Engine::WorldTransform* target);
	/// <summary>カメラの通常追従対象を設定する。</summary>
	/// <param name="target">対象への非所有ポインター。追従解除時はnullptr。</param>
	void SetTarget(const Engine::WorldTransform* target);
	/// <summary>カメラシェイク機能を取得する。</summary>
	/// <returns>CameraControllerが所有する機能への非所有ポインター。</returns>
	CameraShake* GetShake(){ return shake.get(); }
	/// <summary>カメラズーム機能を取得する。</summary>
	/// <returns>CameraControllerが所有する機能への非所有ポインター。</returns>
	CameraZoom* GetZoom(){ return zoom.get(); }
	/// <summary>ロックオン機能を取得する。</summary>
	/// <returns>CameraControllerが所有する機能への非所有ポインター。</returns>
	CameraLockOn* GetCameraLockOn() { return lockOn.get(); }
	/// <summary>通常追従時の先読み設定を置き換える。</summary>
	/// <param name="data">速度方向の先読み量と補間設定。</param>
	void SetLookAheadData(const CameraLookAheadData& data);
	/// <summary>通常追従時の速度ズーム設定を置き換える。</summary>
	/// <param name="data">速度範囲、ズーム距離、補間設定。</param>
	void SetSpeedZoomData(const CameraSpeedZoomData& data);
	/// <summary>攻撃演出などで一時的な先読みを要求する。</summary>
	/// <param name="data">一時適用する先読み設定。</param>
	/// <param name="duration">適用時間（秒）。0以下の場合は適用しない。</param>
	void RequestLookAhead(const CameraLookAheadData& data, float duration);
	/// <summary>攻撃演出などで一時的な速度ズームを要求する。</summary>
	/// <param name="data">一時適用する速度ズーム設定。</param>
	/// <param name="duration">適用時間（秒）。0以下の場合は適用しない。</param>
	void RequestSpeedZoom(const CameraSpeedZoomData& data, float duration);
	/// <summary>攻撃演出などで一時的な注視点オフセットを要求する。</summary>
	/// <param name="data">オフセット量、開始時間、継続時間を含む設定。</param>
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
	/// <summary>現在速度を指定範囲の0.0～1.0へ正規化する。</summary>
	/// <param name="minSpeed">割合0.0に対応する速度。</param>
	/// <param name="maxSpeed">割合1.0に対応する速度。</param>
	/// <returns>範囲内へ制限した速度割合。範囲が無効な場合は0.0。</returns>
	float CalculateSpeedRate(float minSpeed, float maxSpeed) const;
	/// <summary>現在の推定速度から追従先読みオフセットを計算する。</summary>
	/// <param name="data">計算に使用する先読み設定。</param>
	/// <returns>ワールド空間の先読みオフセット。</returns>
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
