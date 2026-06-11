#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
#include <map>
#include <vector>

namespace Engine {
	class WorldTransform;
}


class CameraManager;		// カメラ管理
class BaseCamera;			// カメラ
namespace Character {
	class BaseCharacter;		// キャラクター
}
class BaseWeapon;			// 武器
class EffectSystem;			// エフェクト管理

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
		// コンボシステムが持つ追従先Transform一覧を設定する
		void SetParentTransforms(const std::map<std::string, Engine::WorldTransform*>& parentTransforms) { parentTransforms_ = parentTransforms; }

		// トレイルするか
		bool IsEffectTrail(float timer) const {
			return data_.trailEffectStartTime <= timer && 
				timer <= (data_.trailEffectLifeTime + data_.trailEffectStartTime);
		}
		GloblEffectData& GetData() { return data_; }
	private:
		// 指定時間内のコンボエフェクトを頻度に応じて発生させる
		void EmitComboEffects(float timer);
		// エフェクトの発生基準位置を取得する
		Vector3 GetEffectBasePosition(const ComboEffectEntry& entry) const;
	private:
		GloblEffectData data_;


		Character::BaseCharacter* owner = nullptr;	// 使用者
		BaseWeapon* weapon = nullptr;
		EffectSystem* effectSystem = nullptr;		// エフェクト発生先
		std::map<std::string, Engine::WorldTransform*> parentTransforms_;	// 追従先Transform一覧
		std::vector<float> nextEmitTimes_;			// 各コンボエフェクトの次回発生時間
	};
}
