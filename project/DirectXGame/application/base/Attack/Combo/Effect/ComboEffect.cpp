#include "ComboEffect.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include <DirectXGame/application/base/Character/Base/CharacterContext.h>
#include <DirectXGame/application/base/Character/Move/Base/MoveComponent.h>
#include <DirectXGame/application/base/Camera/Base/CameraManeger.h>
#include"DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include <DirectXGame/engine/3d/Object/Object3d.h>
#include <DirectXGame/engine/Transform/WorldTransform/WorldTransform.h>
#include <algorithm>

namespace Combo {

#pragma region ComboCamera

	// 開始
	void ComboCamera::Enter(Character::BaseCharacter* owner) {
		// 攻撃開始ごとに一回だけ発生するカメラ演出の状態を初期化する
		isCameraChanged_ = false;
		isZoomRequested_ = false;
		isShakeRequested_ = false;
		isTargetOffsetRequested_ = false;
		isLookAheadRequested_ = false;
		isSpeedZoomRequested_ = false;
		isLockOnReleased_ = false;
		cameraManager = owner ? owner->GetCameraManager() : nullptr;
		camera = cameraManager ? cameraManager->GetBaseCamera() : nullptr;
		if (camera && data_.isLockOn) {
			// 攻撃中だけ対象を注視するため、ロックオン用の補間と引き継ぎ設定を渡す
			camera->GetCameraController()->GetCameraLockOn()->GetData() =
				CameraLockOnData{ target,data_.lockOnInterpolation,data_.isLockOn,data_.isLockOnRotate };
			camera->LockOn(target);
		}
	}

	// 更新
	void ComboCamera::Update(float timer, float dt) {
		if (!cameraManager) {
			return;
		}
		camera = cameraManager->GetBaseCamera();
		if (!camera) {
			return;
		}

		// 指定時間になったら、攻撃演出用のカメラへ一回だけ切り替える
		if (data_.isChangeCamera && !isCameraChanged_ && data_.changeCameraStartTime <= timer &&
			!data_.cameraName.empty() && data_.cameraName != "no") {
			cameraManager->SetUseCamera(data_.cameraName, data_.interpolation);
			camera = cameraManager->GetBaseCamera();
			isCameraChanged_ = true;
			if (!camera) {
				return;
			}
		}

		// 終了時間が設定されている場合だけ、攻撃中のロックオンを解除する
		if (data_.lockOnEndTime > 0.0f && data_.lockOnEndTime <= timer && data_.isLockOn && !isLockOnReleased_) {
			camera->LockOn(nullptr);
			isLockOnReleased_ = true;
		}

		bool isZoom =  data_.isZoom;
		if (data_.isLockOnZoom) {
			isZoom = data_.isLockOn && target;
		}

		// 指定時間になったら、攻撃の寄り演出を一回だけ開始する
		if (data_.zoomStartTime <= timer && isZoom && !isZoomRequested_) {
			camera->GetCameraController()->GetZoom()->Request({ data_.zoomTargetDistance,data_.zoomSpeed,data_.zoomDuration });
			isZoomRequested_ = true;
		}

		// 指定時間になったら、攻撃の衝撃用シェイクを一回だけ開始する
		if (data_.isShake && !isShakeRequested_ && data_.shakeStartTime <= timer) {
			Vector3 shakeOffset = data_.shakeOffset;
			if (shakeOffset.x == 0.0f && shakeOffset.y == 0.0f && shakeOffset.z == 0.0f) {
				shakeOffset = { data_.shakeCameraPower,data_.shakeCameraPower,data_.shakeCameraPower };
			}
			if (data_.shakeDuration > 0.0f) {
				camera->GetCameraController()->GetShake()->Request({ data_.shakeDuration,shakeOffset });
			}
			isShakeRequested_ = true;
		}

		// 指定時間になったら、攻撃中だけ注視点をずらして構図を作る
		if (data_.isActionTargetOffset && !isTargetOffsetRequested_ &&
			data_.actionTargetOffsetStartTime <= timer) {
			camera->GetCameraController()->RequestActionTargetOffset({
				data_.actionTargetOffset,
				data_.actionTargetOffsetBlendSpeed,
				data_.actionTargetOffsetDuration
				});
			isTargetOffsetRequested_ = true;
		}

		// 指定時間になったら、移動方向への先読みを一時的に強くする
		if (data_.isLookAhead && !isLookAheadRequested_ && data_.lookAheadStartTime <= timer) {
			camera->GetCameraController()->RequestLookAhead({
				true,
				data_.lookAheadDistance,
				data_.lookAheadMinSpeed,
				data_.lookAheadMaxSpeed,
				data_.lookAheadSmoothSpeed
				}, data_.lookAheadDuration);
			isLookAheadRequested_ = true;
		}

		// 指定時間になったら、速度に応じて一時的にカメラを引く
		if (data_.isSpeedZoom && !isSpeedZoomRequested_ && data_.speedZoomStartTime <= timer) {
			camera->GetCameraController()->RequestSpeedZoom({
				true,
				data_.speedZoomMinSpeed,
				data_.speedZoomMaxSpeed,
				data_.speedZoomNearOffsetZ,
				data_.speedZoomFarOffsetZ,
				data_.speedZoomSmoothSpeed
				}, data_.speedZoomDuration);
			isSpeedZoomRequested_ = true;
		}
	}

	// 攻撃が命中した瞬間に、コンボごとに設定されたシェイクとズームを再生する
	void ComboCamera::OnHit() {
		if (!data_.isHitCameraEffect || !cameraManager) {
			return;
		}

		// カメラ切り替え後も現在使用中のカメラへ確実に演出を適用する
		camera = cameraManager->GetBaseCamera();
		if (!camera) {
			return;
		}

		// 命中の衝撃をカメラ基準の揺れとして再生する
		if (data_.isHitShake && data_.hitShakeDuration > 0.0f) {
			camera->GetCameraController()->GetShake()->Request({ data_.hitShakeDuration, data_.hitShakeOffset });
		}

		// 命中時だけ一時的にカメラ距離を変えて打撃感を強調する
		if (data_.isHitZoom && data_.hitZoomDuration > 0.0f) {
			camera->GetCameraController()->GetZoom()->Request({
				data_.hitZoomTargetDistance,
				data_.hitZoomSpeed,
				data_.hitZoomDuration
				});
		}
	}

	// 終了
	void ComboCamera::Exit() {
		if (cameraManager && cameraManager->GetBaseCamera()) {
			// 攻撃が終わったら攻撃用ロックオンを解除して通常操作へ戻す
			cameraManager->GetBaseCamera()->LockOn(nullptr);
			cameraManager->GetBaseCamera()->GetCameraController()->ClearActionAssist();
		}
		camera = nullptr;
	}

#pragma endregion // コンボカメラ


#pragma region ComboEffect

	// 開始
	void ComboEffect::Enter(Character::BaseCharacter* owner) {
		// コンボエフェクト発生に使う所有者とエフェクト管理を保持する
		this->owner = owner;
		effectSystem = owner ? owner->GetEffect() : nullptr;
		nextEmitTimes_.clear();
		emittedFlags_.clear();
		for (const ComboEffectEntry& entry : data_.comboEffects) {
			nextEmitTimes_.push_back(entry.startTime);
			emittedFlags_.push_back(false);
		}
		wasOnGround_ = owner && owner->GetMoveComponent() && owner->GetMoveComponent()->GetIsLanding();
		// 武器情報取得
		weapon = owner ? owner->GetWeapon() : nullptr;
		if (!weapon) {
			return;
		}
		// トレイル終了
		weapon->SetTrailEmit(false);
		// コンボ演出として武器表示を切り替える
		weapon->GetObject3D()->SetIsDraw(data_.weaponDraw);
	}

	// 更新
	void ComboEffect::Update(const Character::CharacterContext& ctx, float timer, float dt) {
		// 指定時間になったコンボエフェクトを発生させる
		EmitComboEffects(ctx, timer);
		wasOnGround_ = ctx.onGround;

		if (!weapon) {
			return;
		}

		// トレイル使用可能か
		bool isTrail = false;
		if (IsEffectTrail(timer)) { isTrail = true; }
		// トレイルを出すか設定
		weapon->SetTrailEmit(isTrail);
	}

	// 終了
	void ComboEffect::Exit(Character::BaseCharacter* owner) {
		if (weapon) {
			// トレイル終了
			weapon->SetTrailEmit(false);
			// コンボ終了後は通常表示へ戻す
			weapon->GetObject3D()->SetIsDraw(true);
		}
		// 次のコンボ開始時に発生状態を作り直す
		nextEmitTimes_.clear();
		emittedFlags_.clear();
		effectSystem = nullptr;
		this->owner = nullptr;
		weapon = nullptr;
		wasOnGround_ = false;
	}

	void ComboEffect::EmitComboEffects(const Character::CharacterContext& ctx, float timer) {
		if (!owner || !effectSystem) {
			return;
		}
		if (nextEmitTimes_.size() != data_.comboEffects.size()) {
			nextEmitTimes_.clear();
			emittedFlags_.clear();
			for (const ComboEffectEntry& entry : data_.comboEffects) {
				nextEmitTimes_.push_back(entry.startTime);
				emittedFlags_.push_back(false);
			}
		}

		const bool landingTriggered = !wasOnGround_ && ctx.onGround;
		for (int i = 0; i < static_cast<int>(data_.comboEffects.size()); ++i) {
			ComboEffectEntry& entry = data_.comboEffects[i];
			const float interval = (std::max)(entry.interval, 0.001f);
			if (entry.effectName.empty()) {
				continue;
			}

			switch (entry.triggerType)
			{
			case ComboEffectTriggerType::kTimer:
				// 指定時間を過ぎた最初の1回だけ発生させる
				if (!emittedFlags_[i] && timer >= entry.startTime) {
					EmitEntry(entry);
					emittedFlags_[i] = true;
				}
				break;
			case ComboEffectTriggerType::kLanding:
				// 受付時間を満たした状態で着地した瞬間に1回だけ発生させる
				if (!emittedFlags_[i] && landingTriggered && IsTriggerTimeValid(entry, timer)) {
					EmitEntry(entry);
					emittedFlags_[i] = true;
				}
				break;
			case ComboEffectTriggerType::kTimeWindow:
			default:
				// 指定時間範囲中は発生頻度ごとに繰り返し発生させる
				if (IsTriggerTimeValid(entry, timer) && timer >= nextEmitTimes_[i]) {
					EmitEntry(entry);
					nextEmitTimes_[i] += interval;
				}
				break;
			}
		}
	}

	bool ComboEffect::IsTriggerTimeValid(const ComboEffectEntry& entry, float timer) const {
		// endTimeがstartTime以下なら、開始後は終了制限なしとして扱う
		if (timer < entry.startTime) {
			return false;
		}
		if (entry.endTime <= entry.startTime) {
			return true;
		}
		return timer <= entry.endTime;
	}

	void ComboEffect::EmitEntry(const ComboEffectEntry& entry) {
		// 追従先の現在位置にオフセットを足した場所へ発生させる
		const Vector3 emitPosition = GetEffectBasePosition(entry) + entry.offset;
		effectSystem->Emit(entry.effectName, emitPosition);
	}

	Vector3 ComboEffect::GetEffectBasePosition(const ComboEffectEntry& entry) const {
		auto it = parentTransforms_.find(entry.parentName);
		if (it != parentTransforms_.end() && it->second) {
			return it->second->GetWorldPosition();
		}
		if (owner) {
			return owner->GetWorldPosition();
		}
		return {};
	}

#pragma endregion //コンボエフェクト

#pragma region Audio

	void ComboAudio::Enter(Character::BaseCharacter* owner){
	
	}

	void ComboAudio::Update(const Character::CharacterContext& ctx, float timer, float dt){
	
	}

	void ComboAudio::Exit(Character::BaseCharacter * owner){
	
	}


#pragma endregion // 音


	
}
