#pragma once
#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"
#include "DirectXGame/application/base/Weapon/Player/PlayerWeapon.h"
#include "DirectXGame/application/base/Weapon/Player/PlayerSubWeapon.h"
#include "DirectXGame/application/base/Special/RangeBombingSpecial.h"
#include "DirectXGame/application/base/Effect/SlowMotion/SlowMotionManager.h"
#include "DodgeSuccessEffect.h"
#include "NormalPlayerState.h"
#include <DirectXGame/application/base/Attack/Input/AttackInputHandler.h>
#include "DirectXGame/engine/Animation/AnimationComponent.h"

namespace Character {

	///< summary>
	/// 自キャラ
	///</summary>
	class NormalPlayer : public BasePlayer {
	public:


		///< summary>
		/// 初期化
		///</summary>
		void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) override;

		///< summary>
		/// 更新
		///</summary>
		void Update() override;

		/// <summary>
		/// エフェクトの描画
		/// </summary>
		void DrawEffect() override {};

		/// <summary>
		/// 描画2d
		/// </summary>
		void Draw2D() override;

		// 移動処理
		void Move() override {};

		// ジャンプ
		void Jump() override;

		// 回避
		void Dodge() override;

		// 回避成功通知
		void OnDodgeSuccess() override;

		// 回避成功後のコンボ受付中か
		bool IsDodgeSuccessComboWindow() const override;

		// 攻撃(弱攻撃)
		void Attack() override;

		/// 攻撃(強攻撃)
		void HeavyAttack() override;

		// 攻撃(スキル)
		void SkillAttack() override;

		// リロード
		void Reload() override;

		void ApplyComboData(Combo::Editor* comboEditor) override {};

		// プレイヤUI取得
		PlayerUI* GetPlayerUI()override { return ui_.get(); };

	private:
		void InitAttack();

		// ロックオン入力とカメラの対象を同期
		void UpdateLockOn();

		// 攻撃要求
		bool RequestAttack(ActionInput input);

	private:
		// ステートマシーン初期化
		void InitStateMachine() override;

		// 調整項目の適用
		void ApplyGlobalVariables() {};
	private:
		std::unique_ptr<PlayerUI> ui_;			// プレイヤー用UI

		void ReloadComboData();

		// ジャンプ可能か
		bool isCanJump = false;
		//必殺技中か
		bool isSpecial = false;
		float dodgeSuccessComboTimer_ = 0.0f;		// 回避成功後コンボの受付残り時間
		float dodgeSuccessComboWindow_ = 0.45f;		// 回避成功後コンボを受け付ける時間
		std::unique_ptr<SlowMotionManager> slowMotionManager_;	// 回避成功などのスロー演出を処理する管理クラス
		std::unique_ptr<DodgeSuccessEffect> dodgeSuccessEffect_;	// 回避成功時のポストエフェクト演出
	};

}
