#pragma once
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"
#include <DirectXGame/application/base/Weapon/Bullet/BulletWeaponManager.h>
#include "DirectXGame/application/base/Special/RangeBombingSpecial.h"
#include "BulletPlayerState.h"

// 前方宣言
class BaseEnemy;

/// <summary>
/// 銃プレイヤー
/// </summary>
class BulletPlayer : public BasePlayer {
public:

	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) override;

	///< summary>
	/// 更新
	///</summary>
	void Update() override;

	/// <summary>
	/// エフェクトの描画
	/// </summary>
	void DrawEffect() override;

	/// <summary>
	/// 描画2d
	/// </summary>
	void Draw2D() override;

	// 移動処理
	void Move() override;

	// ジャンプ
	void Jump() override;

	// 攻撃
	void Attack() override;

	// プレイヤUI取得
	PlayerUI* GetPlayerUI()override { return ui_.get(); };



private:
	// ステートマシーン初期化
	void InitStateMachine() override;

	// 調整項目の適用
	void ApplyGlobalVariables();

	WorldTransform worldCollider_;
	std::unique_ptr<PlayerUI> ui_;			// プレイヤー用UI

	std::unique_ptr<BulletWeaponManager> weaponManager_; // 武器マネージャー
public:
	// ロックオン
	void LockOn();
};