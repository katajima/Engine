#pragma once
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/UI/PlayerUI.h"


// 前方宣言

namespace Combo {
	class Editor;
}

//前方宣言
class FollowCamera;

namespace Character {

	class BaseEnemy;

	/// <summary>
	/// プレイヤクラス
	/// </summary>
	class BasePlayer : public BaseCharacter {
	public:
		virtual ~BasePlayer();

		// 初期化
		virtual void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) = 0;

		// 毎フレーム更新
		virtual void Update() = 0;
		// 描画エフェクト
		virtual void DrawEffect() = 0;
		// 描画2d
		virtual void Draw2D() = 0;
		// 移動
		virtual void Move() = 0;
		// ジャンプ
		virtual void Jump() = 0;
		// 回避
		virtual void Dodge() = 0;
		// 回避成功通知
		virtual void OnDodgeSuccess() override {};
		// 回避成功後のコンボ受付中か
		virtual bool IsDodgeSuccessComboWindow() const override { return false; };

		// ターゲットキャラクターを設定
		void SetTargetCharacters(const std::vector<const BaseCharacter*>& targetCharacters) { this->targetCharacters = targetCharacters; }

		void SetTargetCharacter(const BaseCharacter* targetCharacters) { this->targetCharacters.push_back(targetCharacters); }


		// リロード
		virtual void Reload() {};
		// コンボデータをシーケンサー適応
		virtual void ApplyComboData(Combo::Editor* comboEditor) {};


	public:
		// フォローカメラの設定
		void SetFollowCamera(FollowCamera* followCamera) { this->followCamera = followCamera; }

		// プレイヤUI取得
		virtual PlayerUI* GetPlayerUI() = 0;

		// サブ武器取得
		BaseWeapon* GetSubWeapon() { return subWeapon_.get(); }

		// SPゲージ加算
		void AddSpGauge(int d);
		// SP発動可能？
		bool GetIsSpecial() const;
		/// <summary>被弾後の無敵時間を毎フレーム減らす。</summary>
		/// <param name="dt">秒単位のフレーム時間。</param>
		void UpdateDamageInvincible(float dt);
		/// <summary>現在ダメージ無敵中か取得する。</summary>
		/// <returns>無敵時間が残っている場合はtrue。</returns>
		bool IsDamageInvincible() const;
	protected:
		/// <summary>ダメージ無敵中はHP減算を無視する。</summary>
		/// <param name="damage">今回適用しようとしているダメージ量。</param>
		/// <returns>ダメージを無視する場合はtrue。</returns>
		bool ShouldIgnoreDamage(float damage) const override;
		/// <summary>HPへダメージが入った瞬間に短時間の無敵を開始する。</summary>
		/// <param name="damage">実際に適用されたダメージ量。</param>
		void OnDamageApplied(float damage) override;
	protected:
		std::vector<const BaseCharacter*> targetCharacters;			// 攻撃対象キャラクターリスト
		FollowCamera* followCamera = nullptr;					// フォローカメラ
		bool isCreativeMode = false;							// クリエイティブモードかどうか
		// サブ武器
		std::unique_ptr<BaseWeapon> subWeapon_ = nullptr;
		float damageInvincibleTimer_ = 0.0f;		// 被弾後無敵の残り時間
		float damageInvincibleDuration_ = 1.0f;	// 被弾後に付与する無敵時間

	};
}
