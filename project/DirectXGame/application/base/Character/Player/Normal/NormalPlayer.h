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
/// <summary>
/// NormalPlayerを管理・実装するクラス。
/// </summary>
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

		/// <summary>
		/// 移動処理
		/// </summary>
		void Move() override {};

		/// <summary>
		/// ジャンプ
		/// </summary>
		void Jump() override;

		/// <summary>
		/// 回避
		/// </summary>
		void Dodge() override;

		/// <summary>
		/// 回避成功通知
		/// </summary>
		void OnDodgeSuccess() override;

		/// <summary>
		/// 回避成功後のコンボ受付中か
		/// </summary>
		bool IsDodgeSuccessComboWindow() const override;
		/// <summary>
		/// リロード
		/// </summary>
		void Reload() override;

		void ApplyComboData(Combo::Editor* comboEditor) override {};

		/// <summary>
		/// プレイヤUI取得
		/// </summary>
		PlayerUI* GetPlayerUI()override { return ui_.get(); };

	private:
		void InitAttack();

		/// <summary>
		/// ロックオン入力とカメラの対象を同期
		/// </summary>
		void UpdateLockOn();
	private:
		/// <summary>
		/// ステートマシーン初期化
		/// </summary>
		void InitStateMachine() override;

		/// <summary>
		/// 調整項目の適用
		/// </summary>
		void ApplyGlobalVariables() {};
	
		/// <summary>
		/// コンボデータリロード
		/// </summary>
		void ReloadComboData();
	private:
		// プレイヤー用UI
		std::unique_ptr<PlayerUI> ui_;		
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
