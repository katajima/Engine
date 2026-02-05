#pragma once
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/application/base/Attack/AttackData.h"


class CameraManager;		// カメラ
class BaseCharacter;		// キャラクター
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
		struct Data {
			// 開始時間
			float startTmer = 0.1f;
			// 生存時間
			float lifeTime = 0.5f;
		};


		// 開始
		void Enter(BaseCharacter* owner);

		// 更新
		void Update(float timer, float dt);

		// 終了
		void Exit(BaseCharacter* owner);

		// トレイルするか
		bool IsEffectTrail(float timer) const {
			return data_.startTmer <= timer && timer <= (data_.lifeTime + data_.startTmer);
		}


		// データ取得
		Data& GetData() { return data_; }

	private:
		Data data_;


		BaseWeapon* weapon = nullptr;
	};
}