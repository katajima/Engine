#pragma once
#include "WeaponData.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include <DirectXGame/application/base/Attack/HitBox/HitBox.h>


// 前方宣言
class EffectSystem;
class BulletManager;
namespace Character {
	class BasePlayer;
	class BaseEnemy;
	class BaseCharacter;
}
namespace Engine {
	class EntityManager;
}

// 武器のベースクラス
class BaseWeapon : public IHitReceiver
{
public:
	// 初期化
	virtual void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) = 0;
	// 更新
	virtual void Update() = 0;
	// 描画エフェクト
	virtual void DrawEffect() = 0;
	// 描画2d
	virtual void Draw2D() = 0;
public:
	// 使っているキャラクター設定
	void SetCharacter(Character::BaseCharacter* character) {this->character = character;}
	// タグによるコライダーの有効・無効を設定
	void SetIsCollider(CollisionTag tag ,bool is) { GetObject3D()->GetColliderComponent()->SetEnableByTag(tag, is); };
	// コライダーコンポーネント
	Engine::ColliderComponent* GetColliderComponent() { return objectComponent_->GetColliderComponent(); };
	// オブジェクト3d取得
	Engine::Object3d* GetObject3D() { return objectComponent_->GetObject3D(); }
	// ワールド変換取得
	Engine::WorldTransform& GetWorldTransform() { return objectComponent_->GetWorldTransform(); }
	// 弾管理クラス設定
	void SetBulletManager(BulletManager* bulletManager) { this->bulletManager = bulletManager; }
protected:
	Character::BaseCharacter* character = nullptr;		// 使っているキャラクター
	const Character::BaseCharacter* target = nullptr;			// ターゲット

	BulletManager* bulletManager = nullptr;	// 弾管理クラス
protected:
	std::unique_ptr<ObjectComponent> objectComponent_;	// オブジェクトコンポーネント
	std::unique_ptr<HitBox::HitBoxInstance> hitBox_;
protected:
	Engine::EntityManager* entityManager = nullptr;	// 3Dエンティティマネージャー
	Engine::GlobalVariables* globalVariables = nullptr;	// グローバル変数
	Engine::Camera* camera = nullptr;						// カメラ
	InputSystem* inputSystem = nullptr;					// 入力(使わないならnullptr)
};

// 近距離の武器クラス
class MeleeWeapon : public BaseWeapon
{
public:
	/// 初期化
	virtual void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) = 0;
	/// 更新
	virtual void Update() = 0;
	/// エフェクトの描画
	virtual void DrawEffect() = 0;
	/// 2d描画
	virtual void Draw2D() = 0;
	
public:
	// ヒットストップ時間を取得
	float GetHitStopTime() const { return mellData_.hitStopTime; } 
protected:
	MeleeWeaponData mellData_; // 近距離武器データ

};

// 遠距離の武器クラス
class RangedWeapon : public BaseWeapon
{
public:
	/// 初期化
	virtual void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) = 0;
	/// 更新
	virtual void Update() = 0;
	/// エフェクトの描画
	virtual void DrawEffect() = 0;
	/// 2d描画
	virtual void Draw2D() = 0;
protected:
	RangedWeaponData rengedData_; // 遠距離武器データ
};