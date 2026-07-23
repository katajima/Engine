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
		BasePlayer();
		virtual ~BasePlayer();

		// 初期化
		virtual void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) = 0;

		/// <summary>
		/// 毎フレーム更新
		/// </summary>
		virtual void Update() = 0;
		/// <summary>
		/// 描画エフェクト
		/// </summary>
		virtual void DrawEffect() = 0;
		/// <summary>
		/// 描画2d
		/// </summary>
		virtual void Draw2D() = 0;
		/// <summary>
		/// 移動
		/// </summary>
		virtual void Move() = 0;
		/// <summary>
		/// ジャンプ
		/// </summary>
		virtual void Jump() = 0;
		/// <summary>
		/// 回避
		/// </summary>
		virtual void Dodge() = 0;
		/// <summary>
		/// 回避成功通知
		/// </summary>
		virtual void OnDodgeSuccess() override {};
		/// <summary>
		/// 回避成功後のコンボ受付中か
		/// </summary>
		virtual bool IsDodgeSuccessComboWindow() const override { return false; };

		/// <summary>
		/// ターゲットキャラクターを設定
		/// </summary>
		void SetTargetCharacters(const std::vector<const BaseCharacter*>& targetCharacters) { this->targetCharacters = targetCharacters; }

		void SetTargetCharacter(const BaseCharacter* targetCharacters) { this->targetCharacters.push_back(targetCharacters); }


		/// <summary>
		/// リロード
		/// </summary>
		virtual void Reload() {};
		/// <summary>
		/// コンボデータをシーケンサー適応
		/// </summary>
		virtual void ApplyComboData(Combo::Editor* comboEditor) {};


	public:
		/// <summary>
		/// フォローカメラの設定
		/// </summary>
		void SetFollowCamera(FollowCamera* followCamera) { this->followCamera = followCamera; }

		/// <summary>
		/// プレイヤUI取得
		/// </summary>
		virtual PlayerUI* GetPlayerUI() = 0;

		/// <summary>
		/// サブ武器取得
		/// </summary>
		BaseWeapon* GetSubWeapon() { return subWeapon_.get(); }

		/// <summary>
		/// SPゲージ加算
		/// </summary>
		void AddSpGauge(int d);
		/// <summary>
		/// SP発動可能？
		/// </summary>
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
