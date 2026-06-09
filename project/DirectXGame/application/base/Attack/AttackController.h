#pragma once
#include <DirectXGame/application/base/Attack/Hit/HitCounter.h>
#include <DirectXGame/application/base/Attack/Combo/Base/ComboSystem.h>
#include <DirectXGame/application/base/Attack/HitBox/HitBoxSystem.h>
#include <DirectXGame/application/base/Attack/LockOn/LockOnSystem.h>
// 前方宣言
namespace Character {
	class ParameterComponent;
}
class BaseWeapon;	
namespace Engine {
	class EntityManager;
	class GlobalVariables;
}


/// <summary>
/// 攻撃状態、コンボ、ヒットカウント、ロックオンをまとめて管理するクラス。
/// </summary>
class AttackController {
public:
	// 攻撃に必要な各サブシステムを初期化する
	void Initialize(Engine::EntityManager* entity3DManager, Engine::GlobalVariables* globalVariables,
		Character::ParameterComponent* base, Character::BaseCharacter* owner);
	// 攻撃状態に応じてヒットカウンターとコンボを更新する
	void Update(const Character::CharacterContext& ctx);

	
public:
	// 攻撃ヒットカウンタークラス取得
	HitCounter& GetHitCounter() { return hitCounter_; }
	// 攻撃パラメーター補正クラス取得
	CombatStat* GetCombatStat() { return combatStat_.get(); }
	// コンボシステムクラス取得
	Combo::System* GetComboSystem() { return comboSystem_.get(); }
	// ロックオンシステム取得
	LockOnSystem* GeyLockOnSysutem() { return lockOnSystem_.get(); }

	// 攻撃中か取得
	bool IsAttack() const { return isAttack_; }
	// 攻撃中か設定
	void SetIsAttack(bool is) { isAttack_ = is; };

	// ヒットカウンターの時間更新を止めるか設定
	void IsStopHitTimer(bool is) { isStopHitTimer_ = is; };

public:
	// デバッグ中か設定
	void SetIsDebug(bool isDebug) { isDebugEditor_ = isDebug; };

private:
	// 攻撃ヒットカウンター
	HitCounter hitCounter_;
	// 攻撃パラメーター補正
	std::unique_ptr<CombatStat> combatStat_;	
	// コンボシステム
	std::unique_ptr<Combo::System> comboSystem_;
	// ロックオンシステム
	std::unique_ptr<LockOnSystem> lockOnSystem_;
private:
	// 攻撃中か
	bool isAttack_ = false;
	// ヒットカウンターの経過時間を止めるか
	bool isStopHitTimer_ = false;

private:
	// 攻撃設定やデバッグ編集で使うグローバル変数
	Engine::GlobalVariables* globalVariables = nullptr;


	// デバッグ中か
	bool isDebugEditor_ = false;

};
