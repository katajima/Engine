#pragma once
#include "DirectXGame/application/base/BaseClass/Character/Player/BasePlayer.h"
// プレイヤー
#include "DirectXGame/application/base/Character/Player/Effect/PlayerEffect.h"


#include "DirectXGame/application/base/Weapon/playerWeapon.h"
#include "DirectXGame/application/base/Special/RangeBombingSpecial.h"
#include "DirectXGame/application/base/Special/BulletSpecial.h"

///< summary>
/// 自キャラ
///</summary>

class Enemy;
class Player : public BasePlayer{
public:
	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Input* input,Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables ,Vector3 position, Camera* camera) override;

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
	
	// エフェクト 
	std::unique_ptr<PlayerEffect> effect_ = std::make_unique<PlayerEffect>();
	
	Object3d objectBody_;					// オブジェクト本体
public:
	void LockOn(const std::vector<BaseEnemy*>& enemys);
private:
	//const int MaxLockOn = 10;	// 最大ロックオン数
	//bool isLockOn = false;		// ロックオン中かどうか
	//std::vector<BaseEnemy*> lockedOnEnemies;// ロックオンした敵
};