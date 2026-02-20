#pragma once
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"



#include "DirectXGame/application/base/UI/PlayerUI.h"
#include"DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include"DirectXGame/application/base/Special/Base/BaseSpecial.h"

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
	class BasePlayer : public BaseCharacter
	{
	public:
		// 初期化
		virtual void Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
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
		// 攻撃(弱攻撃)
		virtual void Attack() = 0;
		// 攻撃(強攻撃)
		virtual void HeavyAttack() {};
		/// 攻撃(スキル攻撃)
		virtual void SkillAttack() {};



		// ターゲットキャラクターを設定
		void SetTargetCharacters(const std::vector<BaseCharacter*>& targetCharacters) { this->targetCharacters = targetCharacters; }

		// リロード
		virtual void Reload() {};
		// コンボデータをシーケンサー適応
		virtual void ApplyComboData(Combo::Editor* comboEditor) {};


	public:
		// フォローカメラの設定
		void SetFollowCamera(FollowCamera* followCamera) { this->followCamera = followCamera; }

		// プレイヤUI取得
		virtual PlayerUI* GetPlayerUI() = 0;

		// SPゲージ加算
		void AddSpGauge(int d) { special_->AddGauge(d); };
		// SP発動可能？
		bool GetIsSpecial() const { return special_->GetIsSpecial(); }

	protected:
		std::vector<BaseCharacter*> targetCharacters;			// 攻撃対象キャラクターリスト
		FollowCamera* followCamera = nullptr;					// フォローカメラ
		bool isCreativeMode = false;							// クリエイティブモードかどうか
	};
}
