#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include "LocomotionContext.h"
/// <summary>
/// 移動リクエストを元に最終的な移動速度→トランスフォーム更新をこなうクラス
/// </summary>
class MovementSystem {
public:

	/// <summary>
	/// 更新
	/// </summary>
	void Initialize();


	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	void Update(const LocomotionContext& cxt,const MoveCommand& cmd,Engine::WorldTransform& world,Engine::RigidBodyComponent& rigid);

public:

	// 着地状態か？
	bool IsOnGround() const{ return onGround_; }

	// ジャンプの処理を使うか設定
	void SetUseGravity(bool isUse) { useGravity = isUse; }

private:

	// 重力処理
	void GravityProess(const LocomotionContext& cxt,Engine::WorldTransform& world,Engine::RigidBodyComponent& rigid);

private:
	// 高さ
	float groundHeight_ = -3.0f;
	Engine::WorldTransform* world;
	Engine::RigidBodyComponent* rigid;

	bool onGround_ = false;
	bool useGravity = true;
};