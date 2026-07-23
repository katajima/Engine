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
	/// <summary>
	/// 攻撃に必要なヒットカウンター、コンボ、ロックオンなどのサブシステムを初期化します。
	/// </summary>
	/// <param name="entity3DManager">攻撃対象などのエンティティを管理するマネージャーです。</param>
	/// <param name="globalVariables">攻撃設定やデバッグ設定を取得するグローバル変数管理です。</param>
	/// <param name="base">攻撃パラメーター補正に使用するキャラクターパラメーターです。</param>
	/// <param name="owner">この攻撃コントローラーを所有するキャラクターです。</param>
	void Initialize(Engine::EntityManager* entity3DManager, Engine::GlobalVariables* globalVariables,
		Character::ParameterComponent* base, Character::BaseCharacter* owner);
	/// <summary>
	/// キャラクターの攻撃状態に応じて、ヒットカウンター、コンボ、ロックオン関連の処理を更新します。
	/// </summary>
    /// <param name="ctx">更新対象キャラクターと周辺システムへの参照をまとめたコンテキストです。</param>
	void Update(const Character::CharacterContext& ctx);


public:
	/// <summary>
	/// 攻撃ヒットカウンターへの参照を取得します。
	/// </summary>
	/// <returns>内部で管理しているヒットカウンターへの参照です.</returns>
	HitCounter& GetHitCounter() { return hitCounter_; }
	/// <summary>
	/// 攻撃パラメーター補正オブジェクトを取得します。
	/// </summary>
	/// <returns>攻撃パラメーター補正オブジェクトへのポインタです.</returns>
	CombatStat* GetCombatStat() { return combatStat_.get(); }
	/// <summary>
	/// コンボシステムを取得します。
	/// </summary>
	/// <returns>内部で管理しているコンボシステムへのポインタです.</returns>
	Combo::System* GetComboSystem() { return comboSystem_.get(); }
	/// <summary>
	/// ロックオンシステムを取得します。
	/// </summary>
	/// <returns>内部で管理しているロックオンシステムへのポインタです。</returns>
	LockOnSystem* GetLockOnSystem() { return lockOnSystem_.get(); }

	/// <summary>
	/// キャラクターが現在攻撃中か取得します。
	/// </summary>
	/// <returns>攻撃中ならtrue、それ以外はfalseです.</returns>
	bool IsAttack() const { return isAttack_; }
	/// <summary>
	/// 攻撃中フラグを設定します。
	/// </summary>
    /// <param name="is">対象の状態を有効にする場合はtrueです。</param>
	void SetIsAttack(bool is) { isAttack_ = is; };

	/// <summary>
	/// ヒットカウンターの経過時間更新を停止するか設定します。
	/// </summary>
    /// <param name="is">対象の状態を有効にする場合はtrueです。</param>
	void IsStopHitTimer(bool is) { isStopHitTimer_ = is; };

public:
	/// <summary>
	/// デバッグ用の挙動を有効にするか設定します。
	/// </summary>
    /// <param name="isDebug">デバッグ中として扱う場合はtrueです。</param>
	void SetIsDebug(bool isDebug) { isDebugEditor_ = isDebug; };

	/// <summary>
	/// デバッグ用の挙動が有効か取得します。
	/// </summary>
	/// <returns>デバッグ中ならtrue、それ以外はfalseです.</returns>
	bool IsDebug() const { return isDebugEditor_; };

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
