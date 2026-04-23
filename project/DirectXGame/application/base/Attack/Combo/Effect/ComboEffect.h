#pragma once
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/application/base/Attack/AttackData.h"
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"

class CameraManager;		// カメラ
namespace Character {
	class BaseCharacter;		// キャラクター
}
class BaseWeapon;			// 武器

namespace Combo {
	/// <summary>
	/// コンボ時のカメラ
	/// </summary>
	class ComboCamera {
	public:
		// データ構造体
		struct Data
		{
			std::string cameraName_ = "no";			// カメラ名
			std::string baseCameraName_ = "no";		// 元のカメラ名
			bool isChangeCamera_ = false;			// カメラを変更するか
			float interpolation_ = 0.0f;			// 補間
		};

		// 開始
		void Enter();

		// 更新
		void Update(float timer, float dt);

		// 終了
		void Exit();

		// カメラ管理設定
		void SetCameraManager(CameraManager* camera) { cameraManager = camera; }

		// データ構造体取得
		Data& GetData();

	private:
		Data data_;
		CameraManager* cameraManager = nullptr;	// カメラ管理
	};

	/// <summary>
	/// コンボ用エフェクト
	/// </summary>
	class ComboEffect {
	public:
		// 開始
		void Enter(Character::BaseCharacter* owner);

		// 更新
		void Update(float timer, float dt);

		// 終了
		void Exit(Character::BaseCharacter* owner);

		// トレイルするか
		bool IsEffectTrail(float timer) const {
			return data_.trailEffectStartTime <= timer && 
				timer <= (data_.trailEffectLifeTime + data_.trailEffectStartTime);
		}
		GloblEffectData& GetData() { return data_; }
	private:
		GloblEffectData data_;


		BaseWeapon* weapon = nullptr;
	};
}