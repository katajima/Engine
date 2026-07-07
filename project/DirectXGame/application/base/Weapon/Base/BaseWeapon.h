#pragma once
#include "WeaponData.h"
#include <DirectXGame/engine/Collider/ColliderData.h>
#include <DirectXGame/engine/struct/Vector3.h>
#include"DirectXGame/engine/collider/CollisionTypeIdDef.h"

#include <memory>
// 前方宣言
class EffectSystem;
class BulletManager;
class ObjectComponent;
class InputSystem;
namespace Character {
	class BasePlayer;
	class BaseEnemy;
	class BaseCharacter;
}
namespace Engine {
	class EntityManager;
	class GlobalVariables;
	class Object3d;
	class WorldTransform;
	class Camera;
	class ColliderComponent;
	class TrailEffect;
}
namespace HitBox {
	class HitBoxInstance;
}

// 武器のベースクラス
class BaseWeapon : public IHitReceiver
{
public:
	BaseWeapon();
	virtual ~BaseWeapon();

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
	void SetCharacter(Character::BaseCharacter* character);
	// タグによるコライダーの有効・無効を設定
	void SetIsCollider(CollisionTag tag, bool is);
	// コライダーコンポーネント
	Engine::ColliderComponent* GetColliderComponent();
	// オブジェクト3d取得
	Engine::Object3d* GetObject3D();
	// ワールド変換取得
	Engine::WorldTransform& GetWorldTransform();
	// 弾管理クラス設定
	void SetBulletManager(BulletManager* bulletManager);
	// 武器に紐づくトレイルを設定
	void SetTrailEffect(Engine::TrailEffect* trailEffect);
	// 武器に紐づくトレイルを取得
	Engine::TrailEffect* GetTrailEffect();
	// トレイル発生のON/OFF
	void SetTrailEmit(bool isEmit);
protected:
	Character::BaseCharacter* character = nullptr;		// 使っているキャラクター
	const Character::BaseCharacter* target = nullptr;			// ターゲット

	BulletManager* bulletManager = nullptr;	// 弾管理クラス
protected:
	std::unique_ptr<ObjectComponent> objectComponent_ = nullptr;	// オブジェクトコンポーネント
	std::unique_ptr<HitBox::HitBoxInstance> hitBox_ = nullptr;
protected:
	Engine::EntityManager* entityManager = nullptr;	// 3Dエンティティマネージャー
	Engine::GlobalVariables* globalVariables = nullptr;	// グローバル変数
	Engine::Camera* camera = nullptr;						// カメラ
	Engine::TrailEffect* trailEffect_ = nullptr;			// EntityManagerが所有するトレイルへの参照
	InputSystem* inputSystem = nullptr;					// 入力(使わないならnullptr)
};

// 近距離の武器クラス
class MeleeWeapon : public BaseWeapon
{
public:
	virtual ~MeleeWeapon();
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
	virtual ~RangedWeapon();
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
	RangedWeaponData rangedData_; // 遠距離武器データ
};
