#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFanctions.h"



#include "DirectXGame/engine/collider/3d/Collider.h"
#include"DirectXGame/engine/collider/ContactRecord.h"
#include "DirectXGame/engine/collider/3d/ColliderComponent.h"

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

	



	Object3d& GetObject3D() { return *objectWeapon_.get(); }

	// カメラのビュープロジェクション
	//void SetCamera(const Camera* camera) { camera_ = camera; };

	void SetIsCollider(bool is) { colliderComponent_->SetEnableByTag(CollisionTag::PlayerAttack, is); };
	void SetColliderHistoryClear() { colliderComponent_->contactRecord_.Clear(); }


	void SetOffset(const Vector3& translation) { offset = translation; }

	// 接触履歴を抹消
	void ContactRecordClear();

	//void SetRad(float rad) { Collider::SetRadius(rad); };

	void SetPlayer(Player* player);
	
	void SetPlayer(BasePlayer* player);

	ColliderComponent* GetColliderComponent() { return colliderComponent_.get(); }

private:
	// 武器
	std::unique_ptr<Object3d> objectWeapon_;
	//std::unique_ptr<WorldTransform> colliderWorld_ = nullptr;
	WorldTransform colliderWorld_;

	const Camera* camera_ = nullptr;
	ContactRecord contactRecord_;
	Vector3 offset;

	Player* player_;
	BasePlayer* basePlayer_;


	std::unique_ptr<ColliderComponent> colliderComponent_ = nullptr;
};

