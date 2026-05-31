#include "ComboEffect.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include <DirectXGame/application/base/Camera/Base/CameraManeger.h>
#include"DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include <DirectXGame/engine/3d/Object/Object3d.h>

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
		// 武器情報取得
		weapon = owner->GetWeapon();
		// トレイル終了
		weapon->SetTrailEmit(false);
	}

	// 更新
	void ComboEffect::Update(float timer, float dt) {
		// トレイル使用可能か
		bool isTrail = false;
		if (IsEffectTrail(timer)) { isTrail = true; }
		// トレイルを出すか設定
		weapon->SetTrailEmit(isTrail);
	}

	// 終了
	void ComboEffect::Exit(Character::BaseCharacter* owner) {
		// トレイル終了
		weapon->SetTrailEmit(false);
	}

#pragma endregion //コンボエフェクト

}
