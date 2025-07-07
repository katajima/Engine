#pragma once
#include "DirectXGame/application/base/BaseClass/Character/Player/BasePlayer.h"
// プレイヤー
#include"BasePlayerState.h"
#include "DirectXGame/application/base/Character/Player/Effect/PlayerEffect.h"
#include "DirectXGame/application/base/Character/Player/UI/PlayerUI.h"


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

private: //Behavior

	//通常行動初期化
	void BehaviorRootInitialize();
	//通常行動更新
	void BehaviorRootUpdate();
	//攻撃行動初期化
	void BehaviorAttackInitialize();
	//攻撃行動更新
	void BehaviorAttackUpdate();
	//死亡行動初期化
	void BehaviorDieInitialize();
	//死亡行動更新
	void BehaviorDieUpdate();

private: // 攻撃関係
	bool IsSpecialAttack;// RT
		
	// 攻撃再発動時間
	float recastTime = 0;
	const float MaxRecastTime = 1.0f;

	// 調整項目の適用
	void ApplyGlobalVariables();

private: // 移動
	// ジャンプ
	void Jump();

private:
	//  プレイヤー用UI
	std::unique_ptr<PlayerUI> ui_ = std::make_unique<PlayerUI>();
	// エフェクト 
	std::unique_ptr<PlayerEffect> effect_ = std::make_unique<PlayerEffect>();
	

	Object3d objectBody_;					// オブジェクト本体
	Object3d* objectReticle_;				// オブジェクトレティクル
	float reticleRad_ = 100.0f;				// レティクルの半径　　　
	Vector3 rangeBombingPos{};				// レンジボムの位置
public:
	void LockOn(const std::vector<BaseEnemy*>& enemys);
private:
	const int MaxLockOn = 10;	// 最大ロックオン数
	bool isLockOn = false;		// ロックオン中かどうか
	std::vector<BaseEnemy*> lockedOnEnemies;// ロックオンした敵
};