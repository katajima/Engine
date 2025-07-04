#pragma once
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/BaseClass/Character/Player/BasePlayer.h"
// プレイヤー
#include"BasePlayerState.h"
#include "DirectXGame/application/base/Character/Player/Effect/PlayerEffect.h"
#include "DirectXGame/application/base/Character/Player/UI/PlayerUI.h"
//#include "DirectXGame/application/base/Character/Player/Attack/PlayerAttackFactory.h"
//#include"DirectXGame/application/base/Attack/AttackManager.h"

#include "DirectXGame/application/base/Weapon/playerWeapon.h"

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
	//では必殺技でフェーズ1で選択、フェーズ2で発射(上空に)、フェーズ3で(的へ(上付近まで))、フェーズ4で(的に向かい着弾))
	void BehaviorDieUpdate();

public: // 攻撃関係
	struct AttackKeyFlag
	{
		bool IsNormalAttack; // B
		bool IsSpecialAttack;// RT
	};
	
private: // 攻撃関係
	// 攻撃用ワーク
	struct WrokAttack {
		AttackKeyFlag key; // 攻撃方法キー
	};
	WrokAttack workAttack{};
	
		
	// 攻撃再発動時間
	float recastTime = 0;
	const float MaxRecastTime = 1.0f;

	// 攻撃キー入力
	void AttackKey();
	// 攻撃タイプ
	void AttackTypes();
	
	//攻撃
	void Attack();

	// 調整項目の適用
	void ApplyGlobalVariables();

	void SetAttackCombo();

public:
	void LockOn(const std::vector<BaseEnemy*>& enemys);

	const int MaxLockOn = 10;	// 最大ロックオン数
	bool isLockOn = false;		// ロックオン中かどうか
private: // 移動
	// 移動処理
	void Move();
	// ジャンプ
	void Jump();
public:
	AttackTypePlay GetAttackType() const { return weapon_->GetAttackTypePlay(); };
private:
	//  プレイヤー用UI
	std::unique_ptr<PlayerUI> ui_ = std::make_unique<PlayerUI>();
	// エフェクト 
	std::unique_ptr<PlayerEffect> effect_ = std::make_unique<PlayerEffect>();
	// 攻撃マネージャー
	//std::unique_ptr<AttackManager> attackManager_;
	// 攻撃ファクトリー
	//std::unique_ptr<BaseAttackFactory> playerAttackFactory_;
	//
	std::unordered_map<std::string, WorldTransform*> transformMap;

	Object3d objectBody_;					// オブジェクト本体
	std::unique_ptr<Primitive> primitiveCylinder_ = nullptr; // 円柱プリミティブ
	Object3d* objectReticle_;				// オブジェクトレティクル
	float reticleRad_ = 100.0f;				// レティクルの半径　　　
	Vector3 rangeBombingPos{};				// レンジボムの位置
	bool isCreativeMode = false;			// クリエイティブモードかどうか
private:
	std::vector<BaseEnemy*> lockedOnEnemies;// ロックオンした敵
};