#pragma once
#include "WeaponData.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include <DirectXGame/application/base/Attack/HitBox/HitBox.h>

class BaseCharacter; // 前方宣言

// 武器のベースクラス
class BaseWeapon : public IHitReceiver
{
public:
	// 初期化
	virtual void Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) = 0;
	// 更新
	virtual void Update() = 0;
	// 描画エフェクト
	virtual void DrawEffect() = 0;
	// 描画2d
	virtual void Draw2D() = 0;
public:
	// 使っているキャラクター設定
	void SetCharacter(BaseCharacter* character) {
		this->character = character;
	};
	// タグによるコライダーの有効・無効を設定
	void SetIsCollider(CollisionTag tag ,bool is) { GetObject3D()->GetColliderComponent()->SetEnableByTag(tag, is); };
	// 攻撃中かどうかのフラグを取得
	bool IsActive() const { return data_.isActive; };
	// オートマチックかどうかのフラグを取得
	bool IsAutomatic() const { return data_.isAutomatic; };
	// 攻撃中かどうかのフラグを設定
	void SetIsActive(bool isActive) { data_.isActive = isActive; };
	
	// コライダーコンポーネント
	Engine::ColliderComponent* GetColliderComponent() { return objectComponent_->GetColliderComponent(); };
	// オブジェクト3d取得
	Engine::Object3d* GetObject3D() { return objectComponent_->GetObject3D(); }
	// ワールド変換取得
	Engine::WorldTransform& GetWorldTransform() { return objectComponent_->GetObject3D()->GetWorldTransform(); }

	// リキャストタイム取得
	bool GetIsRecastTimeOver() const { return data_.MaxRecastTime <= data_.recastTime; }
	// リキャストタイム設定
	void RecastTime(float timer) { data_.recastTime += timer; }


	//HitBox* GetHitBox() { return hitBox_.get();}
public:
	
protected:
	WeaponData data_;	// 武器データ
protected:
	BaseCharacter* character;	// 使っているキャラクター
protected:
	std::unique_ptr<ObjectComponent> objectComponent_;	// オブジェクトコンポーネント
	std::unique_ptr<HitBox> hitBox_;
protected:
	Engine::Entity3DManager* entity3DManager_ = nullptr;	// 3Dエンティティマネージャー
	Engine::Entity2DManager* entity2DManager_ = nullptr;	// 2Dエンティティマネージャー
	Engine::GlobalVariables* globalVariables_ = nullptr;	// グローバル変数
	Engine::Camera* camera_ = nullptr;						// カメラ
	Engine::Input* input_ = nullptr;						// 入力(使わないならnullptr)
	Engine::Audio* audio_ = nullptr;
};

// 近距離の武器クラス
class MeleeWeapon : public BaseWeapon
{
public:
	/// 初期化
	virtual void Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
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

// 前方宣言
class BulletManager;

// 遠距離の武器クラス
class RangedWeapon : public BaseWeapon
{
public:
	/// 初期化
	virtual void Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) = 0;
	/// 更新
	virtual void Update() = 0;
	/// エフェクトの描画
	virtual void DrawEffect() = 0;
	/// 2d描画
	virtual void Draw2D() = 0;
	// 弾管理クラス設定
	void SetBulletManager(BulletManager* bulletManager) { bulletManager_ = bulletManager; }

protected:
	RangedWeaponData rengedData_; // 遠距離武器データ
	BulletManager* bulletManager_ = nullptr;	// 弾管理クラス
};