#pragma once
#include <DirectXGame/application/base/Attack/Hit/HitCounter.h>
#include <DirectXGame/application/base/Attack/Combo/ComboSystem.h>
#include <DirectXGame/application/base/Attack/HitBox/HitBoxSystem.h>

// 前方宣言
class CharacterParameterComponent;
class BaseCharacter;
class BaseWeapon;	
namespace Engine {
	class Entity3DManager;
	class GlobalVariables;
}


/// <summary>
/// 攻撃を管理するクラス。
/// </summary>
class AttackController {
public:
	// 初期化
	void Initialize(Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables,
		CharacterParameterComponent* base, BaseCharacter* owner);
	// 更新
	void Update(float dt);

	
public:
	// 攻撃ヒットカウンタークラス取得
	HitCounter& GetHitCounter() { return hitCounter_; }
	// 攻撃パラメーター補正クラス取得
	CombatStat* GetCombatStat() { return combatStat_.get(); }
	// コンボシステムクラス取得
	ComboSystem* GetComboSystem() { return comboSystem_.get(); }
	// ヒットボックスシステム取得
	HitBoxSystem* GetHitBoxSystem() { return hitBoxSystem_.get(); };
	// 攻撃中か取得
	bool IsAttack() const { return isAttack_; }
	// 攻撃中か設定
	void SetIsAttack(bool is) { isAttack_ = is; };

	// treu
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
	std::unique_ptr<ComboSystem> comboSystem_;
	// ヒットボックスシステム
	std::unique_ptr<HitBoxSystem> hitBoxSystem_;

private:
	// 攻撃中か
	bool isAttack_ = false;
	//
	bool isStopHitTimer_ = false;

private:
	Engine::GlobalVariables* globalVariables = nullptr;


	// デバッグ中か
	bool isDebugEditor_ = false;

};
