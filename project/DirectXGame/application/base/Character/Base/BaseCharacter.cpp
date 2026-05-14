#include "BaseCharacter.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include <DirectXGame/application/base/Attack/Response/Response.h>
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"
#include <DirectXGame/application/base/Attack/AttackController.h>
#include "DirectXGame/application/base/Bullet/base/BulletSpawn.h" 
#include <DirectXGame/application/base/Character/Death/DeathSystem.h>
#include"DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include"DirectXGame/application/base/Special/Base/BaseSpecial.h"

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
		parameterComponent_->HP().Add(-damage);		// HPをダメージ分減算
		if (GetHP() <= 0) {
			parameterComponent_->HP().value = 0.0f;
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
	// オブジェクト3d取得
	ObjectComponent* BaseCharacter::GetObjectComponentShadow() { return objectComponentShadow_.get(); }
	// ワールド変換取得
	Engine::WorldTransform& BaseCharacter::GetWorldTransform() { return objectComponent_->GetWorldTransform(); }
	// ワールド座標取得
	Vector3 BaseCharacter::GetWorldPosition() const { return objectComponent_->GetWorldTransform().GetWorldPosition(); }
}