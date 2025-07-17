#pragma once
#include "DirectXGame/application/base/BaseClass/Character/Player/BasePlayer.h"
#include "DirectXGame/application/base/Weapon/playerWeapon.h"
#include "DirectXGame/application/base/Special/RangeBombingSpecial.h"
#include "DirectXGame/application/base/Special/BulletSpecial.h"

///< summary>
/// 自キャラ
///</summary>

class Enemy;
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


	void Draw2D() override;

	// 移動処理
	void Move() override;

	// ジャンプ
	void Jump() override;

private: // 攻撃関係

	// 調整項目の適用
	void ApplyGlobalVariables();
private:
};