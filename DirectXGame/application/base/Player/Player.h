#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include"DirectXGame/engine/base/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/input/Input.h"
#include <imgui.h>
#include <list>




#include "DirectXGame/engine/effect/Particle/ParticleManager.h"
#include "DirectXGame/engine/effect/Particle/ParticleEmitter.h"
#include "DirectXGame/engine/collider/3d/Collider.h"
#include "DirectXGame/engine/effect/Trail/TrailEffect.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

// プレイヤー
#include"BasePlayerState.h"
#include"playerWeapon.h"
#include "DirectXGame/application/base/Player/Effect/PlayerEffect.h"
#include "DirectXGame/application/base/Player/UI/PlayerUI.h"
#include "DirectXGame/application/base/Player/Special/BulletSpecial.h"

///< summary>
/// 自キャラ
///</summary>

class Enemy;
class FollowCamera;
class DirectXCommon;
class Entity3DManager;
class Entity2DManager;
class BulletManager;

class Player : public Collider {
public:


	///< summary>
	/// 初期化
	///</summary>
	void Initialize(DirectXCommon* dxcommon, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager,Vector3 position, Camera* camera);

	///< summary>
	/// 更新
	///</summary>
	void Update();

	///< summary>
	/// 描画
	///</summary>
	void Draw();

	void DrawP();
	
	void Draw2D();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision([[maybe_unused]] Collider* other) override;

	virtual Vector3 GetCenterPosition() const;

	
	// 振るまい
	enum class Behavior {
		kRoot,   // 通常状態
		kAttack, // 攻撃中
		kJump,   // ジャンプ中
		kDie,       // 死亡状態
	};
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


	//振るまい
	Behavior behavior_ = Behavior::kRoot;
	// 次の振るまいリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;
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
		bool IsDashAttack;   // X
		bool IsJampAttack;   // Y
		bool IsSpecialAttack;// RT
	};
	enum class  AttackType
	{
		kNone = 0,
		kNormal, // 通常攻撃
		kDash,   // ダッシュ攻撃
		kJamp,   // ジャンプ攻撃
		kSpecial,// 必殺技攻撃
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
		AttackType type = AttackType::kNone;
		// 次の振るまいリクエスト
		std::optional<AttackType> typeRequest_ = std::nullopt;
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

	const int MaxLockOn = 10;
	bool isLockOn = false;


private: // 移動
	// 移動処理
	void Move();
	// 重力
	void Gravity();
	// 移動加算
	void AddMove();
	// 移動制限
	void LimitMove();

	float graVelo;

	float groundY = 2;

	bool isJamp = false;

	float ty = 0;

	bool isMove = false;

	
public:
	
	Object3d& GetObject3D() { return objectBase_; }
	
	
	playerWeapon* GetWeapon() { return weapon_.get(); }

	Behavior GetBehavior() const { return behavior_; };
	AttackType GetAttackType() const { return workAttack.type; };

	uint32_t GetSerialNumber() const { return serialNumber; }

	bool GetAlive() const { return isAlive; };
	
	void AddDamege(float da) { hp -= int(da); };

	void AddSpecial(int d) { bulletSpecial_->AddGauge(d); };

	bool GetInvincible() const { return isInvincible; }
	
	bool GetIsSpecial() const { return bulletSpecial_->GetIsSpecial(); }

	int GetHitCount() const { return workAttack.hitCount; }

	void AddHit() { workAttack.hitCount++; };
	void AddSP() { bulletSpecial_->AddGauge(1); };

public:

	// dxCommon
	void SetDxCommon(DirectXCommon* dxcommon) { dxCommon_ = dxcommon; }

	// カメラのビュープロジェクション
	void SetCamera(Camera* camera) { camera_ = camera; };


	void SetHitTime() { workAttack.hitTime = 1.5f; }

	void SetBulletManager(BulletManager* bulletManager) { bulletManager_ = bulletManager;};

	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }

	void SetInput(Input* input) {
		input_ = input;
		bulletSpecial_->SetInput(input);
	};

private:  // パラメータ
	
	uint32_t maxHp = 100;
	int hp = 100;
	bool isAlive = true;
	float damage_ = 0;
	bool isInvincible = false;
private:
	// スペシャル攻撃
	std::unique_ptr<BulletSpecial> bulletSpecial_;
	//  プレイヤー用UI
	std::unique_ptr<PlayerUI> ui_ = std::make_unique<PlayerUI>();
	// エフェクト 
	std::unique_ptr<PlayerEffect> effect_ = std::make_unique<PlayerEffect>();
	// 武器
	std::unique_ptr<playerWeapon> weapon_;



	// オブジェクト3D

	Object3d objectBase_;
	// 本体
	Object3d objectBody_;
	
	//　レティクル
	Object3d objectReticle_;
	

	// 移動関連
	// 速度
	Vector3 velocity_ = {};
	float moveLimit = 200;
	float speed;

	


	

	ContactRecord contactRecord_;
	// シリアルナンバー
	uint32_t serialNumber = 0;

private:
	DirectXCommon* dxCommon_;
	Entity3DManager* entity3DManager_;
	BulletManager* bulletManager_;
	FollowCamera* followCamera_;
	Camera* camera_ = nullptr;
	Input* input_;
	std::vector<BaseEnemy*> lockedOnEnemies;
};


