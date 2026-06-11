#include "ComboEffect.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
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
		cameraManager = owner->GetCameraManager();
		if (data_.isLockOn) {
			cameraManager->GetBaseCamera()->GetCameraController()->
				GetCameraLockOn()->GetData() = CameraLockOnData{nullptr,data_ .lockOnInterpolation,data_.isLockOn };
			cameraManager->GetBaseCamera()->LockOn(target);
		}
	}

	// 更新
	void ComboCamera::Update(float timer, float dt) {
		
		// ロックオン処理
		if (data_.lockOnInterpolation <= timer && data_.isLockOn) {
			cameraManager->GetBaseCamera()->LockOn(nullptr);

		}

		bool isZoom =  data_.isZoom;
		if (data_.isLockOnZoom) {
			isZoom = data_.isLockOn && target;
		}

		// ズーム処理
		if (data_.zoomStartTime <= timer && isZoom)
			
			cameraManager->GetBaseCamera()->GetCameraController()->GetZoom()->
			Request({ data_.zoomTargetDistance,data_.zoomSpeed,data_.zoomDuration });
	}

	// 終了
	void ComboCamera::Exit() {
		cameraManager->GetBaseCamera()->LockOn(nullptr);
	}

#pragma endregion // コンボカメラ


#pragma region ComboEffect

	// 開始
	void ComboEffect::Enter(Character::BaseCharacter* owner) {
		// コンボエフェクト発生に使う所有者とエフェクト管理を保持する
		this->owner = owner;
		effectSystem = owner ? owner->GetEffect() : nullptr;
		nextEmitTimes_.clear();
		for (const ComboEffectEntry& entry : data_.comboEffects) {
			nextEmitTimes_.push_back(entry.startTime);
		}
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
	void ComboEffect::Update(float timer, float dt) {
		// 指定時間になったコンボエフェクトを発生させる
		EmitComboEffects(timer);

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
		effectSystem = nullptr;
		this->owner = nullptr;
		weapon = nullptr;
	}

	void ComboEffect::EmitComboEffects(float timer) {
		if (!owner || !effectSystem) {
			return;
		}
		if (nextEmitTimes_.size() != data_.comboEffects.size()) {
			nextEmitTimes_.clear();
			for (const ComboEffectEntry& entry : data_.comboEffects) {
				nextEmitTimes_.push_back(entry.startTime);
			}
		}

		for (int i = 0; i < static_cast<int>(data_.comboEffects.size()); ++i) {
			ComboEffectEntry& entry = data_.comboEffects[i];
			const float endTime = (std::max)(entry.startTime, entry.endTime);
			const float interval = (std::max)(entry.interval, 0.001f);
			if (entry.effectName.empty() || timer < entry.startTime || timer > endTime || timer < nextEmitTimes_[i]) {
				continue;
			}

			// 追従先の現在位置にオフセットを足した場所へ発生させる
			const Vector3 emitPosition = GetEffectBasePosition(entry) + entry.offset;
			effectSystem->Emit(entry.effectName, emitPosition);
			nextEmitTimes_[i] += interval;
		}
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

}
