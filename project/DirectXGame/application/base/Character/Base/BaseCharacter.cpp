#include "BaseCharacter.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/engine/Entity/ObjectComponent.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include <DirectXGame/application/base/Attack/Response/Response.h>
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"
#include <DirectXGame/application/base/Attack/AttackController.h>
#include "DirectXGame/application/base/Bullet/base/BulletSpawn.h" 
#include <DirectXGame/application/base/Character/Death/DeathSystem.h>
#include"DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include"DirectXGame/application/base/Special/Base/BaseSpecial.h"
#include <algorithm>
#include <utility>

namespace Character {
	BaseCharacter::BaseCharacter() = default;
	BaseCharacter::~BaseCharacter() = default;

	void BaseCharacter::InitializeBaseAddItem() {
		AddItem("HP", parameterComponent_->parameters->HP.value);
		AddItem("MaxHP", parameterComponent_->parameters->HP.maxValue);
		AddItem("MP", parameterComponent_->parameters->MP.value);
		AddItem("MaxMP", parameterComponent_->parameters->MP.maxValue);
		AddItem("stamina", parameterComponent_->parameters->stamina.value);
		AddItem("MaxStamina", parameterComponent_->parameters->stamina.maxValue);



		parameterComponent_->parameters->HP.value = GetValue<float>("HP");
		parameterComponent_->parameters->HP.maxValue = GetValue<float>("MaxHP");
		parameterComponent_->parameters->MP.value = GetValue<float>("MP");
		parameterComponent_->parameters->MP.maxValue = GetValue<float>("MaxMP");
		parameterComponent_->parameters->stamina.value = GetValue<float>("stamina");
		parameterComponent_->parameters->stamina.maxValue = GetValue<float>("MaxStamina");
	}

	void BaseCharacter::UpdateBaseGetValue() {}

	// 攻撃リクエスト
	bool BaseCharacter::RequestAttack(ActionInput input) {
		auto* ac = GetAttackController();
		if (ac && ac->GetComboSystem()) {
			return ac->GetComboSystem()->RequestAttack(input);
		}
		return false;
	}

	// 名前取得
	std::string BaseCharacter::GetName() const { return objectComponent_->GetName(); }

	// キャラクタータイプ設定
	void BaseCharacter::SetCharacterType(Type type) { parameterComponent_->characterType_ = type; }
	// キャラクター取得
	Type BaseCharacter::GetCharacterType() const { return parameterComponent_->characterType_; }
	// キャラクターの生存状態を取得
	bool BaseCharacter::GetAlive() const { return objectComponent_->GetObjectStateFlags().isAlive; }
	// キャラクターの生存状態を取得
	void BaseCharacter::SetAlive(bool is) { objectComponent_->GetObjectStateFlags().isAlive = is; }
	// HP取得
	float BaseCharacter::GetHP() const { return parameterComponent_->parameters->HP.value; }
	// ダメージ
	void BaseCharacter::AddDamage(float damage) {
		// 共通防御判定で無敵、パリィ、ガード中のダメージを先に遮断する
		const GameAction::DefenseResult defenseResult = ResolveActionDefense();
		if (defenseResult == GameAction::DefenseResult::Invincible ||
			defenseResult == GameAction::DefenseResult::Parried ||
			defenseResult == GameAction::DefenseResult::Guarded) {
			// 直接ダメージ系の弾でも、ガード系アクションイベントを発行する
			if (defenseResult == GameAction::DefenseResult::Parried) {
				EmitActionEvent({ GameAction::EventType::Parried, this, nullptr });
			}
			else if (defenseResult == GameAction::DefenseResult::Guarded) {
				EmitActionEvent({ GameAction::EventType::Guarded, this, nullptr });
			}
			return;
		}
		// 無敵時間などでダメージを受けない状態なら、HPと被弾後処理を変更しない。
		if (ShouldIgnoreDamage(damage)) {
			return;
		}

		const float hpBefore = GetHP();				// ダメージ適用前のHP
		parameterComponent_->HP().Add(-damage);		// HPをダメージ分減算
		if (GetHP() <= 0) {
			parameterComponent_->HP().value = 0.0f;
		}

		// 実際にHPが減った時だけ、派生クラスへ被弾後処理を通知する。
		if (damage > 0.0f && GetHP() < hpBefore) {
			OnDamageApplied(damage);
		}
	}
	// 攻撃属性付きダメージ
	void BaseCharacter::ApplyAttackDamage(float damage, AttackAttribute attribute) {
		const float hpBefore = GetHP();	// ダメージ前のHP
		AddDamage(damage);				// 既存のHP減算と下限処理を使用

		// 生存中からHP0になった瞬間だけ、死亡原因の攻撃属性を記録する。
		if (hpBefore > 0.0f && GetHP() <= 0.0f) {
			fatalAttackAttribute_ = attribute;
		}
	}

	void BaseCharacter::UpdateActionDefense(const CharacterContext& ctx) {
		// ガード入力は毎フレーム更新し、入力を離した瞬間にはガードを解除する
		actionDefense_.guarding = ctx.isGuarding;
		if (ctx.guardTrigger && ctx.isGuarding) {
			// ガード開始直後だけパリィ受付時間を付与する
			actionDefense_.parryTimer = 0.12f;
		}
		else {
			// パリィ受付時間は実時間で減少させる
			actionDefense_.parryTimer = (std::max)(0.0f, actionDefense_.parryTimer - ctx.dt);
		}
		if (!actionDefense_.guarding) {
			// ガードを離した後はパリィ受付も終了する
			actionDefense_.parryTimer = 0.0f;
		}
		// アクション由来の一時無敵時間を減算する
		actionDefense_.actionInvincibleTimer = (std::max)(0.0f, actionDefense_.actionInvincibleTimer - ctx.dt);
	}

	void BaseCharacter::SetActionDefenseFlags(bool superArmor, bool invincible, bool guardPoint, float guardDamageScale) {
		// 現在ノードの防御設定を共通状態へ反映する
		actionDefense_.superArmor = superArmor;
		actionDefense_.invincible = invincible;
		actionDefense_.guardPoint = guardPoint;
		actionDefense_.guardDamageScale = (std::clamp)(guardDamageScale, 0.0f, 1.0f);
	}

	void BaseCharacter::SetActionInvincibleTime(float seconds) {
		// 回避やスキルなど、ステートをまたぐ一時無敵時間を共通タイマーへ設定する
		actionDefense_.actionInvincibleTimer = (std::max)(seconds, 0.0f);
	}

	void BaseCharacter::ClearActionDefenseFlags() {
		// ノード終了時に攻撃由来の防御効果だけを解除する
		actionDefense_.superArmor = false;
		actionDefense_.invincible = false;
		actionDefense_.guardPoint = false;
		actionDefense_.guardDamageScale = 0.0f;
	}

	GameAction::DefenseResult BaseCharacter::ResolveActionDefense() const {
		// 無敵時間またはアクション無敵はすべての被弾を無効化する
		if (actionDefense_.invincible || actionDefense_.actionInvincibleTimer > 0.0f) {
			return GameAction::DefenseResult::Invincible;
		}
		// ガード開始直後はパリィを優先する
		if (actionDefense_.guarding && actionDefense_.parryTimer > 0.0f) {
			return GameAction::DefenseResult::Parried;
		}
		// ガード入力または攻撃ノードのガードポイントを防御成功として扱う
		if (actionDefense_.guarding || actionDefense_.guardPoint) {
			return GameAction::DefenseResult::Guarded;
		}
		// スーパーアーマーはダメージを受けるが、被弾リアクションを抑制する
		if (actionDefense_.superArmor) {
			return GameAction::DefenseResult::SuperArmored;
		}
		return GameAction::DefenseResult::Hit;
	}

	void BaseCharacter::AddActionEventListener(GameAction::EventListener listener) {
		// 空のコールバックは登録せず、イベント通知時の分岐を減らす
		if (listener) {
			actionEventListeners_.push_back(std::move(listener));
		}
	}

	void BaseCharacter::ClearActionEventListeners() {
		// キャラクター破棄やシーン切り替え時に購読者を解放する
		actionEventListeners_.clear();
	}

	void BaseCharacter::EmitActionEvent(const GameAction::Event& event) {
		// 登録順を維持してすべてのアクションイベント購読者へ通知する
		for (const auto& listener : actionEventListeners_) {
			if (listener) {
				listener(event);
			}
		}
	}
	// 削除フラグ
	bool BaseCharacter::GetDelete() const { return objectComponent_->GetObjectStateFlags().isDeleted; };
	// 削除する
	void BaseCharacter::Delete() { objectComponent_->GetObjectStateFlags().isDeleted = true; };
	// 時間
	float BaseCharacter::GetTime() { return objectComponent_->GetTime(); }
	// 移動出来るか設定
	void BaseCharacter::IsMove(bool is) { isMove = is; }
	// 移動可能か
	bool BaseCharacter::GetIsMove() const { return isMove; }
	// パラメータ取得
	BasicParameters* BaseCharacter::GetBasicParameters() const { return parameterComponent_->parameters.get(); }
	// 基本パラメータ
	BasicParameters* BaseCharacter::Parameters() { return parameterComponent_->parameters.get(); }

	// 保存生成
	void  BaseCharacter::CreateGroup(const std::string name) {
		objectComponent_->SetName(name);
		globalVariables->SetGroupCategory(name, "Character/Parameter");
		globalVariables->CreateGroup(name);
	}


	// キャラクターステートマシーン取得
	CharacterStateMachine* BaseCharacter::GetCharacterStateMachine() { return stateMachine_.get(); }
	// 現在の状態取得
	CharacterMainState BaseCharacter::GetCurrentMainState() const { return stateMachine_->GetCurrentMainState(); }
	// 過去のステート
	CharacterMainState BaseCharacter::GetPrevState() const { return stateMachine_->GetPrevState(); }
	// 必殺技取得
	BaseSpecial* BaseCharacter::GetSpecial() { return special_.get(); }
	// 武器取得
	BaseWeapon* BaseCharacter::GetWeapon() { return weapon_.get(); }
	// 弾の出現
	BulletSpawn* BaseCharacter::GetBulletSpawn() { return bulletSpawn_.get(); }
	// 死亡システム取得
	DeathSystem* BaseCharacter::GetDeathSystem() {
		return deathSystem_.get();
	};
	// コライダーコンポーネント
	Engine::ColliderComponent* BaseCharacter::GetColliderComponent() { return objectComponent_->GetColliderComponent(); };
	// オブジェクト3d取得
	ObjectComponent* BaseCharacter::GetObjectComponent() { return objectComponent_.get(); }
	// ワールド変換取得
	Engine::WorldTransform& BaseCharacter::GetWorldTransform() { return objectComponent_->GetWorldTransform(); }
	//
	// ワールド変換取得
	const Engine::WorldTransform* BaseCharacter::GetConstWorldTransform() const { return &objectComponent_->GetWorldTransform(); };
	// ワールド座標取得
	Vector3 BaseCharacter::GetWorldPosition() const { return objectComponent_->GetWorldTransform().GetWorldPosition(); }
	// 入力をセット
	void BaseCharacter::SetInputSystem(InputSystem* inputSystem){
		this->inputSystem = inputSystem;
		contextSystem_->SetInputSystem(inputSystem);
	}
	// カメラ設定
	void BaseCharacter::SetCamera(Engine::Camera* camera){	this->camera = camera;	}
}
