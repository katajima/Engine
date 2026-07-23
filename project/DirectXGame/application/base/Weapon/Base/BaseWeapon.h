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
/// <summary>
/// BaseWeaponを管理・実装するクラス。
/// </summary>
class BaseWeapon : public IHitReceiver
{
public:
	BaseWeapon();
	virtual ~BaseWeapon();

	/// <summary>武器オブジェクト、当たり判定、描画依存を初期化する。</summary>
	/// <param name="inputSystem">武器操作に使用する入力。敵武器ではnullptrを許容する。</param>
	/// <param name="entityManager">武器オブジェクトとコンポーネントの生成元。</param>
	/// <param name="globalVariables">武器調整値の登録・保存先。</param>
	/// <param name="position">武器の初期ワールド座標。</param>
	/// <param name="camera">武器描画に使用する非所有カメラ。</param>
	virtual void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) = 0;
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;
	/// <summary>
	/// 描画エフェクト
	/// </summary>
	virtual void DrawEffect() = 0;
	/// <summary>
	/// 描画2d
	/// </summary>
	virtual void Draw2D() = 0;
public:
	/// <summary>この武器を使用するキャラクターを設定する。</summary>
	/// <param name="character">所有者への非所有ポインター。武器利用中は有効であること。</param>
	void SetCharacter(Character::BaseCharacter* character);
	/// <summary>指定衝突タグに対する武器コライダーの有効状態を設定する。</summary>
	/// <param name="tag">有効状態を変更する衝突対象タグ。</param>
	/// <param name="is">衝突を有効にする場合はtrue。</param>
	void SetIsCollider(CollisionTag tag, bool is);
	/// <summary>武器のコライダーコンポーネントを取得する。</summary>
	/// <returns>内部ObjectComponentが所有する非所有ポインター。未生成の場合はnullptr。</returns>
	Engine::ColliderComponent* GetColliderComponent();
	/// <summary>武器の描画オブジェクトを取得する。</summary>
	/// <returns>EntityManagerが所有する非所有ポインター。</returns>
	Engine::Object3d* GetObject3D();
	/// <summary>武器のワールド変換を取得する。</summary>
	/// <returns>武器の生存期間中有効な変更可能参照。</returns>
	Engine::WorldTransform& GetWorldTransform();
	/// <summary>遠距離武器が生成する弾の登録先を設定する。</summary>
	/// <param name="bulletManager">非所有ポインター。弾を生成する間は有効であること。</param>
	void SetBulletManager(BulletManager* bulletManager);
	/// <summary>武器軌跡として使用するトレイルを設定する。</summary>
	/// <param name="trailEffect">EntityManagerなどが所有する非所有ポインター。解除時はnullptr。</param>
	void SetTrailEffect(Engine::TrailEffect* trailEffect);
	/// <summary>設定済みの武器トレイルを取得する。</summary>
	/// <returns>非所有ポインター。未設定の場合はnullptr。</returns>
	Engine::TrailEffect* GetTrailEffect();
	/// <summary>武器トレイルの頂点生成を開始または停止する。</summary>
	/// <param name="isEmit">生成する場合はtrue。</param>
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
/// <summary>
/// MeleeWeaponを管理・実装するクラス。
/// </summary>
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
	/// <summary>
	/// ヒットストップ時間を取得
	/// </summary>
	float GetHitStopTime() const { return mellData_.hitStopTime; } 
protected:
	MeleeWeaponData mellData_; // 近距離武器データ

};

// 遠距離の武器クラス
/// <summary>
/// RangedWeaponを管理・実装するクラス。
/// </summary>
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
