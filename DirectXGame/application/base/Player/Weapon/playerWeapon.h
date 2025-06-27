#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFanctions.h"


class BasePlayer;
class Player;
class Entity3DManager;
class playerWeapon
{
public:

	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Entity3DManager* entity3DManager,Camera* camera);

	///< summary>
	/// 更新
	///</summary>
	void Update();

	///< summary>
	/// 描画
	///</summary>
	void Draw();

	



	Object3d& GetObject3D() { return *objectWeapon_; }

	// カメラのビュープロジェクション
	//void SetCamera(const Camera* camera) { camera_ = camera; };

	void SetIsCollider(bool is) { objectWeapon_->GetColliderComponent()->SetEnableByTag(CollisionTag::PlayerAttack, is); };
	void SetColliderHistoryClear() { objectWeapon_->GetColliderComponent()->contactRecord_.Clear(); }


	void SetOffset(const Vector3& translation) { offset = translation; }

	// 接触履歴を抹消
	void ContactRecordClear();

	void SetPlayer(Player* player);
	
	void SetPlayer(BasePlayer* player);

	ColliderComponent* GetColliderComponent() { return objectWeapon_->GetColliderComponent(); }

private:
	// 武器
	Object3d* objectWeapon_;
	WorldTransform colliderWorld_;
	uint32_t weaponColliderId_ = 0;

	WorldTransform colliderWorld2_;
	uint32_t weaponColliderId2_ = 0;

	const Camera* camera_ = nullptr;
	ContactRecord contactRecord_;
	Vector3 offset;

	Player* player_;
	BasePlayer* basePlayer_;

};

