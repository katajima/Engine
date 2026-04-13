#pragma once
#include <DirectXGame/application/base/Attack/HitBox/HitBoxSystem.h>
#include "DirectXGame/application/base/Attack/Combo/Input/ComboButton.h"
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"


class MovementComponent;			// ジャンプシステム

namespace Combo {
	/// <summary>
	/// ヒットボックス
	/// </summary>
	class ComboHitBox {
	public:
		~ComboHitBox() { 
			collData_.clear();
			useHitBoxName_.clear();
		}
		// ヒットボックスデータ
		struct Data {
			float hitBpxWindowStart = 1.0f;		// ヒットボックス生成スタート
			float lifeTime = 1.0f;					// ヒットボックス生存時間
			// ヒットボックス使用者タイプ
			HitBox::UseType hitBoxUseType = HitBox::UseType::kPlayer;
			// ヒットボックスの発生条件タイプ
			HitBox::SpawnType spawnType = HitBox::SpawnType::kOnTime;
			// ヒットボックス依存先タイプ
			HitBox::ParentType dependenceType = HitBox::ParentType::kParent;
			// ヒットボックス生存タイプ
			HitBox::LifetimeType lifetimeType = HitBox::LifetimeType::kTimed;
			// ヒットボックス影響タイプ
			HitBox::HitEffectType hitEffectType = HitBox::HitEffectType::kDamageAndForce;
			// ヒット記録を使用するか（使用した場合連続ヒットしない）
			bool useContactRecord = true;				
			// オフセット
			Vector3 offset{};
			// 親子名
			std::string	parentName = "";
		};

		// 開始
		void Enter(Character::BaseCharacter* owner , Type type);

		// 更新
		void Update(const Character::CharacterContext& ctx, float timer);

		// 終了
		void Exit();


	public:
		HitBox::CollData& GetCollData(int index) { return collData_[index]; }
		// データ取得
		Data& GetData() { return data_; }
		// コライダーデータ追加
		void AddCollider(const HitBox::CollData& hitBoxData, const Combo::GlobalData& reaction);
		// 使うヒットボックス名設定
		void AddUseHitBox(const std::string& name) { useHitBoxName_.push_back(name); };
		// 使うヒットボックス名クリーン
		void ClearUseHitBox() { useHitBoxName_.clear(); }
		// 親子設定
		void SetPerent(Engine::WorldTransform* perent) { this->perent = perent; };
		//
		void SetDirection(Vector3 direction) { this->direction = direction; };
	private: // 貰いもの
		// ヒットボックスシステム
		HitBox::System* hitBoxSystem = nullptr;
		// ヒットボックス
		HitBox::HitBoxInstance* hitBox = nullptr;
		// 移動システム
		MovementComponent* movementComponent = nullptr;
		// 親子
		Engine::WorldTransform* perent = nullptr;
		// 方向
		Vector3 direction = {};
		// コンボタイプ
		Type type;
		//
		int32_t id = -1;
	private:
		// ヒットボックスデータ
		Data data_;
		// コライダーデータ
		std::vector<HitBox::CollData> collData_;
		// 使うヒットボックス名
		std::vector<std::string> useHitBoxName_;

	private:
		// ヒットボックス出現
		bool isPopHitBox_ = false;
		//
		Character::BaseCharacter* owner = nullptr;
	};
};
