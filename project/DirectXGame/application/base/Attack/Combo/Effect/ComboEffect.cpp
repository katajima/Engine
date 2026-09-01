#include "ComboEffect.h"
#include "DirectXGame/application/base/Attack/AttackController.h"
#include"DirectXGame/application/base/Character/Base/CharacterManager.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include <DirectXGame/application/base/Character/Base/CharacterContext.h>
#include <DirectXGame/application/base/Character/Move/Base/MoveComponent.h>
#include <DirectXGame/application/base/Camera/Base/CameraManager.h>
#include"DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include <DirectXGame/engine/3d/Object/Object3d.h>
#include <DirectXGame/engine/Transform/WorldTransform/WorldTransform.h>
#include <DirectXGame/engine/Audio/Audio.h>
#include <DirectXGame/engine/Effect/Trail/TrailEffect.h>
#include <algorithm>
#include <cstdint>

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
		this->owner = owner;
		effectSystem = owner ? owner->GetEffect() : nullptr;
		nextEmitTimes_.clear();
		emittedFlags_.clear();
		for (const ComboEffectEntry& entry : data_.comboEffects) {
			nextEmitTimes_.push_back(entry.startTime);
			emittedFlags_.push_back(false);
		}
		wasOnGround_ = owner && owner->GetMoveComponent() && owner->GetMoveComponent()->GetIsLanding();
		hitEvent_ = false;
		missEvent_ = false;
		branchEvent_ = false;
		cancelEvent_ = false;
		currentTimer_ = 0.0f;
		// トレイルの生成はComboEffectが担当し、武器は表示状態の切り替えだけに使用する。
		if (effectSystem && owner) {
			effectSystem->SetCamera(owner->GetCamera());
			CreateTrailEntries();
		}
		weapon = owner ? owner->GetWeapon() : nullptr;
		if (weapon) {
			weapon->GetObject3D()->SetIsDraw(data_.weaponDraw);
		}
	}

	void ComboEffect::Update(const Character::CharacterContext& ctx, float timer, float dt) {
		// パーティクルとトレイルは同じ発生時間リストを共有する。
		currentTimer_ = timer;
		EmitComboEffects(ctx, timer);
		UpdateTrailEntries(ctx, timer);
		wasOnGround_ = ctx.onGround;
		(void)dt;
	}

	void ComboEffect::Exit(Character::BaseCharacter* owner) {
		// このコンボが生成したトレイルを解放する。
		RemoveTrailEntries();
		if (weapon) {
			weapon->GetObject3D()->SetIsDraw(true);
		}
		nextEmitTimes_.clear();
		emittedFlags_.clear();
		effectSystem = nullptr;
		this->owner = nullptr;
		weapon = nullptr;
		wasOnGround_ = false;
		hitEvent_ = false;
		missEvent_ = false;
		branchEvent_ = false;
		cancelEvent_ = false;
		currentTimer_ = 0.0f;
		(void)owner;
	}
	void ComboEffect::NotifyHit() { hitEvent_ = true; }
	void ComboEffect::NotifyMiss() { missEvent_ = true; }
	void ComboEffect::NotifyBranch() { branchEvent_ = true; }
	void ComboEffect::NotifyCancel() { cancelEvent_ = true; }

    void ComboEffect::ClearRuntimeEffects() {
        // StateMachineを破棄するリロードでも、EffectSystem側に残ったトレイルを削除する。
        RemoveTrailEntries();
        if (weapon) {
            weapon->GetObject3D()->SetIsDraw(true);
        }
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
			if (!IsTriggerSatisfied(entry, ctx)) {
				continue;
			}
			if (entry.effectName.empty()) {
				continue;
			}

			switch (entry.triggerType)
			{
			case ComboEffectTriggerType::kTimer:
				// 指定時間を過ぎた最初の1回だけ発生させる
				if (!emittedFlags_[i] && timer >= entry.startTime) {
					EmitEntry(entry, timer);
					emittedFlags_[i] = true;
				}
				break;
			case ComboEffectTriggerType::kLanding:
				// 受付時間を満たした状態で着地した瞬間に1回だけ発生させる
				if (!emittedFlags_[i] && landingTriggered && IsTriggerTimeValid(entry, timer)) {
					EmitEntry(entry, timer);
					emittedFlags_[i] = true;
				}
				break;
			case ComboEffectTriggerType::kHit:
			case ComboEffectTriggerType::kMiss:
			case ComboEffectTriggerType::kBranch:
			case ComboEffectTriggerType::kCancel:
				if (!emittedFlags_[i]) {
					EmitEntry(entry, timer);
					emittedFlags_[i] = true;
				}
				break;
			case ComboEffectTriggerType::kTimeWindow:
			case ComboEffectTriggerType::kHitCount:
			case ComboEffectTriggerType::kGround:
			case ComboEffectTriggerType::kAir:
			case ComboEffectTriggerType::kButton:
			default:
				// 指定時間範囲中は発生頻度ごとに繰り返し発生させる
				if (IsTriggerTimeValid(entry, timer) && timer >= nextEmitTimes_[i]) {
					EmitEntry(entry, timer);
					nextEmitTimes_[i] += interval;
				}
				break;
			}
		}
	}

	bool ComboEffect::IsTriggerSatisfied(const ComboEffectEntry& entry, const Character::CharacterContext& ctx) const {
		switch (entry.triggerType) {
		case ComboEffectTriggerType::kHit: return hitEvent_;
		case ComboEffectTriggerType::kMiss: return missEvent_;
		case ComboEffectTriggerType::kHitCount: return owner && owner->GetAttackController() && owner->GetAttackController()->GetHitCounter().GetHitCount() >= entry.requiredHitCount;
		case ComboEffectTriggerType::kBranch: return branchEvent_;
		case ComboEffectTriggerType::kCancel: return cancelEvent_;
		case ComboEffectTriggerType::kGround: return ctx.onGround;
		case ComboEffectTriggerType::kAir: return !ctx.onGround;
		case ComboEffectTriggerType::kButton:
			switch (entry.inputType) {
			case ComboEffectInputType::kJump: return ctx.inputData.jumpTrigger;
			case ComboEffectInputType::kDodge: return ctx.inputData.dodgeTrigger;
			case ComboEffectInputType::kSkill: return ctx.inputData.skillTrigger;
			case ComboEffectInputType::kSpecial: return ctx.inputData.specialTrigger;
			default: return ctx.inputData.jumpTrigger || ctx.inputData.dodgeTrigger || ctx.inputData.skillTrigger || ctx.inputData.specialTrigger;
			}
		default: return true;
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
	void ComboEffect::EmitEntry(const ComboEffectEntry& entry, float timer) {
		// トレイルはUpdateTrailEntriesで時間に応じて継続制御する。
		if (entry.type == ComboEffectType::Trail || !effectSystem) {
			return;
		}
		// トレイルと同じ軌跡上でパーティクルの発生位置を評価する。
		const float duration = entry.trajectory.duration > 0.0f ? entry.trajectory.duration : (entry.endTime > entry.startTime ? entry.endTime - entry.startTime : 1.0f);
		const float normalizedTime = (timer - entry.startTime) / (std::max)(duration, 0.001f);
		const Vector3 emitPosition = GetTrajectoryPosition(entry, normalizedTime);
		effectSystem->Emit(entry.effectName, emitPosition);
	}

	void ComboEffect::CreateTrailEntries() {
		// コンボ再入場時に前回の実行用トレイルが残らないよう、先に解放する。
		RemoveTrailEntries();
		// トレイル種別のエントリごとに実行用トレイルを1つ生成する。
		trailRuntimeNames_.assign(data_.comboEffects.size(), "");
		for (size_t i = 0; i < data_.comboEffects.size(); ++i) {
			const ComboEffectEntry& entry = data_.comboEffects[i];
			if (entry.type != ComboEffectType::Trail || !effectSystem) {
				continue;
			}
			auto parentIt = parentTransforms_.find(entry.parentName);
			Engine::WorldTransform* parent = parentIt != parentTransforms_.end() ? parentIt->second : nullptr;
			if (!parent && owner) {
				parent = &owner->GetWorldTransform();
			}
			if (!parent) {
				continue;
			}
			const std::string runtimeName = "ComboTrail_" + std::to_string(reinterpret_cast<std::uintptr_t>(this)) + "_" + std::to_string(i);
			trailRuntimeNames_[i] = runtimeName;
			const Matrix4x4 localTransform = MakeAffineMatrix(entry.transformScale, entry.transformRotation, entry.transformPosition);
			const Vector3 trailStart = localTransform.Transform(entry.trailOffsetStart);
			const Vector3 trailEnd = localTransform.Transform(entry.trailOffsetEnd);
			Engine::TrailTrajectorySettings transformedTrajectory = entry.trajectory;
			transformedTrajectory.point0 = localTransform.Transform(entry.trajectory.point0);
			transformedTrajectory.point1 = localTransform.Transform(entry.trajectory.point1);
			transformedTrajectory.point2 = localTransform.Transform(entry.trajectory.point2);
			transformedTrajectory.point3 = localTransform.Transform(entry.trajectory.point3);
			transformedTrajectory.orbitCenter = localTransform.Transform(entry.trajectory.orbitCenter);
			effectSystem->CreateTrailEffect(runtimeName, entry.trailTexture, entry.trailLifeTime, *parent,
				owner ? owner->GetCamera() : nullptr, entry.trailColor, trailStart, trailEnd,
				transformedTrajectory, entry.trailSettings);
			effectSystem->SetTrailEmit(runtimeName, false);
		}
	}

	void ComboEffect::RemoveTrailEntries() {
		// コンボ終了時に、このコンボが生成したトレイルをすべて削除する。
		if (effectSystem) {
			for (const std::string& runtimeName : trailRuntimeNames_) {
				if (!runtimeName.empty()) {
					effectSystem->RemoveTrailEffect(runtimeName);
				}
			}
		}
		trailRuntimeNames_.clear();
	}

	void ComboEffect::UpdateTrailEntries(const Character::CharacterContext& ctx, float timer) {
		// 各トレイルを個別に設定された発生時間範囲で切り替える。
		if (!effectSystem) {
			return;
		}
		for (size_t i = 0; i < data_.comboEffects.size() && i < trailRuntimeNames_.size(); ++i) {
			const ComboEffectEntry& entry = data_.comboEffects[i];
			if (entry.type != ComboEffectType::Trail || trailRuntimeNames_[i].empty()) {
				continue;
			}
			float trailEnd = entry.endTime;
			if (trailEnd <= entry.startTime) {
				trailEnd = entry.startTime + (std::max)(entry.trailLifeTime, 0.001f);
			}
			const bool active = IsTriggerSatisfied(entry, ctx) && timer >= entry.startTime && timer <= trailEnd;
			effectSystem->SetTrailEmit(trailRuntimeNames_[i], active);
		}
	}

	Vector3 ComboEffect::GetTrajectoryPosition(const ComboEffectEntry& entry, float normalizedTime) const {
		// 共有軌道評価処理を使ってパーティクルの発生位置を計算する。
		const Vector3 basePosition = GetEffectBasePosition(entry) + entry.offset;
		const Matrix4x4 localTransform = MakeAffineMatrix(entry.transformScale, entry.transformRotation, entry.transformPosition);
		if (entry.trajectory.type == Engine::TrailTrajectoryType::kNone) {
			return basePosition + entry.transformPosition;
		}
		const Vector3 localPosition = localTransform.Transform(Engine::EvaluateTrailTrajectory(entry.trajectory, normalizedTime));
		auto parentIt = parentTransforms_.find(entry.parentName);
		const Engine::WorldTransform* parent = parentIt != parentTransforms_.end() ? parentIt->second : nullptr;
		if (!parent && owner) {
			parent = &owner->GetWorldTransform();
		}
		if (!parent) {
			return basePosition + localPosition;
		}
		return {
			basePosition.x + parent->worldMat_.m[0][0] * localPosition.x + parent->worldMat_.m[0][1] * localPosition.y + parent->worldMat_.m[0][2] * localPosition.z,
			basePosition.y + parent->worldMat_.m[1][0] * localPosition.x + parent->worldMat_.m[1][1] * localPosition.y + parent->worldMat_.m[1][2] * localPosition.z,
			basePosition.z + parent->worldMat_.m[2][0] * localPosition.x + parent->worldMat_.m[2][1] * localPosition.y + parent->worldMat_.m[2][2] * localPosition.z
		};
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

	void ComboAudio::Initialize(Engine::AudioManager* audioManager) {
		// AudioManagerの所有権はFramework側にあるため、再生に使う参照だけを保持する。
		audioManager_ = audioManager;
	}

	void ComboAudio::Enter(Character::BaseCharacter* owner) {
		// コンボ開始ごとに攻撃音の発火状態を初期化する。
		(void)owner;
		isAttackSoundPlayed_ = false;
	}

	void ComboAudio::Update(const Character::CharacterContext& ctx, float timer, float dt) {
		// ヒットストップ中でも攻撃音を重複再生しないよう、一度だけ発火する。
		(void)ctx;
		(void)dt;
		if (isAttackSoundPlayed_ || timer < data_.attackStartTime) {
			return;
		}
		if (audioManager_ && !data_.attackSoundName.empty()) {
			audioManager_->Play(data_.attackSoundName, false, data_.attackVolume);
		}
		isAttackSoundPlayed_ = true;
	}

	void ComboAudio::Exit(Character::BaseCharacter* owner) {
		// 攻撃の振り終わりや着地など、ノード終了へ割り当てた音を再生する。
		(void)owner;
		if (audioManager_ && !data_.finishSoundName.empty()) {
			audioManager_->Play(data_.finishSoundName, false, data_.finishVolume);
		}
		isAttackSoundPlayed_ = false;
	}

	void ComboAudio::OnHit() {
		// 命中が確定した回数だけヒット音を鳴らし、多段攻撃にも対応する。
		if (audioManager_ && !data_.hitSoundName.empty()) {
			audioManager_->Play(data_.hitSoundName, false, data_.hitVolume);
		}
	}

#pragma endregion // 音


	
}
