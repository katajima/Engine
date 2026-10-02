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
		/// <summary>コライダー設定を読み取り専用で取得します。</summary>
		/// <returns>内部コライダーデータへの読み取り専用参照です。</returns>
		const HitBox::CollData& GetCollData() const { return collData_; }
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
		// このComboHitBoxが生成した期限付き判定を識別する所有者IDです。
		HitBox::HitBoxOwnerId hitBoxOwnerId_ = 0;
		//
		int32_t id = -1;
	private:
		// コライダーデータ
		HitBox::CollData collData_;
	private:
		// 生成済みの判定波数です。
		std::uint32_t spawnedCount_ = 0;
		// 次の判定波を生成できるコンボ時間です。
		float nextSpawnTime_ = 0.0f;
		// 最初の発生条件が成立したかどうかです。
		bool spawnStarted_ = false;
		//
		Character::BaseCharacter* owner = nullptr;
	};
};
