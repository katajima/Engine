#pragma once
#include <DirectXGame/application/base/Attack/HitBox/HitBoxSystem.h>
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"


class MovementComponent;			// ジャンプシステム

namespace Combo {
	/// <summary>
	/// ヒットボックス
	/// </summary>
	class ComboHitBox {
	public:
		~ComboHitBox() {}
		/// <summary>
		/// 開始
		/// </summary>
		void Enter(Character::BaseCharacter* owner , Type type);

		/// <summary>
		/// 更新
		/// </summary>
		void Update(const Character::CharacterContext& ctx, float timer);

		/// <summary>
		/// 終了
		/// </summary>
		void Exit();
	public:
		/// <summary>
		/// CollData
		/// </summary>
		HitBox::CollData& GetCollData() { return collData_;}
		/// <summary>
		/// コライダーデータ追加
		/// </summary>
		void AddCollider(const HitBox::CollData& hitBoxData, const Combo::GlobalData& reaction);
		/// <summary>
		/// 親子設定
		/// </summary>
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
		Type type{};
		//
		int32_t id = -1;
	private:
		// コライダーデータ
		HitBox::CollData collData_;
	private:
		// ヒットボックス出現
		bool isPopHitBox_ = false;
		//
		Character::BaseCharacter* owner = nullptr;
	};
};
