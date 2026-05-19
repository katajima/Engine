#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"

namespace Engine {
	class WorldTransform;
}


class CameraManager;		// カメラ管理
class BaseCamera;			// カメラ
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
		// 開始
		void Enter(Character::BaseCharacter* owner);

		// 更新
		void Update(float timer, float dt);

		// 終了
		void Exit();

		void SetTarget(const Engine::WorldTransform* target) { this->target = target; }

		// データ構造体取得
		GlobalCameraData& GetData() { return data_; }
	private:
		const Engine::WorldTransform* target;	

		BaseCamera* camera = nullptr;
		CameraManager* cameraManager = nullptr;	// カメラ管理
		// カメラデータ
		GlobalCameraData data_;
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