#pragma once
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/BaseCharacter.h"

// プレイヤー
#include"BasePlayerState.h"
#include"DirectXGame/application/base/Player/Weapon/playerWeapon.h"
#include "DirectXGame/application/base/Player/Effect/PlayerEffect.h"
#include "DirectXGame/application/base/Player/UI/PlayerUI.h"
#include "DirectXGame/application/base/Player/Special/BulletSpecial.h"
#include "DirectXGame/application/base/Player/Special/RangeBombingSpecial.h"
#include "DirectXGame/application/base/Player/Attack/PlayerAttackFactory.h"


#include"DirectXGame/application/base/Attack/AttackManager.h"

///< summary>
/// 自キャラ
///</summary>

class Enemy;
class FollowCamera;
class BulletManager;
class Player : public BaseCharacter{
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
	struct Parameter
	{
		float t;
		float max_t;
	};
	struct AttackKeyFlag
	{
		bool IsAttack; // 攻撃するか
		bool IsNormalAttack; // B
		bool IsSpecialAttack;// RT
	};
	enum class  AttackTypePlay
	{
		kNone = 0,
		kNormal, // 通常攻撃
		kJump, // ジャンプ攻撃
	};
private: // 攻撃関係
	struct StartEnd
	{
		Vector3 str;
		Vector3 end;
	};

	// 攻撃用ワーク
	struct WrokAttack {
		// 攻撃ギミックの媒介変数
		Parameter attackAll; // 攻撃
		AttackKeyFlag key; // 攻撃方法キー
		float parameter;
		// 
		//振るまい
		AttackTypePlay type = AttackTypePlay::kNone;
		// 次の振るまいリクエスト
		std::optional<AttackTypePlay> typeRequest_ = std::nullopt;
		//
		StartEnd pos; // 位置
		// 過去位置
		Vector3 oldPos;

		// ジャンプ攻撃時の移動ベクトル
		Vector3 velocity;


		int comboIndex = 0;
		int inComboPhase = 0;
		bool comboNext = false;
		int comboSwitchingNum = 0;
		Vector2 joyDirection;
		//
		// ヒットカウント
		int hitCount = 0;
		float hitTime = 0;
	};
	WrokAttack workAttack{};
	
	
	
	// コンボの数
	static const int ComboNum = 4;

	
	// 攻撃再発動時間
	float recastTime = 0;
	const float MaxRecastTime = 1.0f;

	// 攻撃キー入力
	void AttackKey();
	// 攻撃タイプ
	void AttackTypes();
	// 攻撃タイプ初期化
	void AttackTypeInit(int comboIndex);

	//攻撃
	void Attack();

	// 調整項目の適用
	void ApplyGlobalVariables();

	void SetAttackCombo(WrokAttack& work);



public:
	void LockOn(const std::vector<BaseEnemy*>& enemys);

	const int MaxLockOn = 10;	// 最大ロックオン数
	bool isLockOn = false;		// ロックオン中かどうか
private: // 移動
	// 移動処理
	void Move();
	// ジャンプ
	void Jump();
	// 重力
	void Gravity();
	// 移動加算
	void AddMove();
	// 移動制限
	void LimitMove();

	float graVelo;     // 重力の速度
	float groundY = 2; // 地面の高さ
public:
	
	Object3d* GetObject3D() { return objectBase_; }
	
	
	playerWeapon* GetWeapon() { return weapon_.get(); }

	BasicBehavior GetBehavior() const { return basicbehavior_; };
	AttackTypePlay GetAttackType() const { return workAttack.type; };


	void AddDamege(float da) { HP() -= int(da); };

	void AddSpecial(int d) { rangeBombingSpecial_->AddGauge(d); };

	bool GetInvincible() const { return GetSituation().isInvincible; }
	
	bool GetIsSpecial() const { return rangeBombingSpecial_->GetIsSpecial(); }

	int GetHitCount() const { return workAttack.hitCount; }

	void AddHit() { workAttack.hitCount++; };
	void AddSP() { rangeBombingSpecial_->AddGauge(1); };

public:

	// カメラのビュープロジェクション
	void SetCamera(Camera* camera) { camera_ = camera; };


	void SetHitTime() { workAttack.hitTime = 1.5f; }

	void SetBulletManager(BulletManager* bulletManager) { bulletManager_ = bulletManager;};

	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }

	RangeBombingSpecial* GetRangeBombingSpecial() { return rangeBombingSpecial_.get(); }

private:
	// スペシャル攻撃
	std::unique_ptr<BulletSpecial> bulletSpecial_;
	// スペシャル攻撃
	std::unique_ptr<RangeBombingSpecial> rangeBombingSpecial_;
	//  プレイヤー用UI
	std::unique_ptr<PlayerUI> ui_ = std::make_unique<PlayerUI>();
	// エフェクト 
	std::unique_ptr<PlayerEffect> effect_ = std::make_unique<PlayerEffect>();
	// 武器
	std::unique_ptr<playerWeapon> weapon_;
	// 攻撃マネージャー
	std::unique_ptr<AttackManager> attackManager_;
	// 攻撃ファクトリー
	std::unique_ptr<BaseAttackFactory> playerAttackFactory_;
	//
	std::unordered_map<std::string, WorldTransform*> transformMap;

	Object3d objectBody_;					// オブジェクト本体
	std::unique_ptr<Primitive> primitiveCylinder_ = nullptr; // 円柱プリミティブ
	Object3d* objectReticle_;				// オブジェクトレティクル
	float reticleRad_ = 100.0f;				// レティクルの半径　　　
	Vector3 rangeBombingPos{};				// レンジボムの位置
	float moveLimit = 200;					// 移動制限
	bool isCreativeMode = false;			// クリエイティブモードかどうか
private:
	BulletManager* bulletManager_;			// 弾管理
	FollowCamera* followCamera_;			// フォローカメラ
	std::vector<BaseEnemy*> lockedOnEnemies;// ロックオンした敵
};


