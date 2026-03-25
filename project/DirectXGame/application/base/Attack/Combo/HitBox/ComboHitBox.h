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
			useHitBox_.clear();
		}

		enum class Shape {
			kAABB,
			kOBB,
			kSphere
		};


		// ヒットボックスデータ
		struct Data {
			float hitBpxWindowStart_ = 1.0f;		// ヒットボックス生成スタート
			float lifeTime_ = 1.0f;					// ヒットボックス生存時間
			// ヒットボックス使用者タイプ
			HitBox::UseType hitBoxUseType_;
			// ヒットボックスの発生条件タイプ
			HitBox::SpawnType spawnType_ = HitBox::SpawnType::kOnTime;
			// ヒットボックス依存先タイプ
			HitBox::ParentType dependenceType_ = HitBox::ParentType::kParent;
			// オフセット
			Vector3 offset_{};
			// 親子名
			std::string	parentName_ = "";
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
		void AddUseHitBox(const std::string& name) { useHitBox_.push_back(name); };
		// 使うヒットボックス名クリーン
		void ClearUseHitBox() { useHitBox_.clear(); }
		// 親子設定
		void SetPerent(Engine::WorldTransform* perent) { perent_ = perent; };
		//
		void SetDirection(Vector3 direction) { direction_ = direction; };
	private:
		// ヒットボックスシステム
		HitBox::System* hitBoxSystem_ = nullptr;
		// 移動システム
		MovementComponent* movementComponent = nullptr;
		// 親子
		Engine::WorldTransform* perent_ = nullptr;
	private:
		ComboButton button_ = ComboButton(GamePadButton::GAMEPAD_B, ComboButtonInputType::kPressed); // コンボボタン
		// ヒットボックスデータ
		Data data_;
		// コライダーデータ
		std::vector<HitBox::CollData> collData_;

		// 使うヒットボックス名
		std::vector<std::string> useHitBox_;

	private:
		// ヒットボックス出現
		bool isPopHitBox_ = false;
		//
		Vector3 direction_ = {};
		//
		float timer_ = 0.0f;
		// コンボタイプ
		Type type;
	};
};
