#pragma once
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"
#include "DirectXGame/engine/Move/RigidBodyComponent.h"
#include "LocomotionContext.h"
#include "DirectXGame/application/base/Character/Base/CharacterContext.h"

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
	void Update(const Character::CharacterContext& ctx,const MoveCommand& cmd,Engine::WorldTransform& world,Engine::RigidBodyComponent& rigid);

public:
	// 着地状態か？
	bool IsOnGround() const{ return isLinding_; }
	// 
	Vector3 GetDirection() const { return direction_;}
	// ジャンプの処理を使うか設定
	void SetUseGravity(bool isUse) { useGravity = isUse; }
	// 重力速度リセット
	void ResetGravityVelocity();
private:
	// 重力処理
	void GravityProess(const Character::CharacterContext& cxt,Engine::WorldTransform& world,Engine::RigidBodyComponent& rigid);
	// 回転処理
	void RotateProcess(const Character::CharacterContext& cxt, Engine::WorldTransform& world);
private:
	// 高さ
	float groundHeight_ = -3.0f;
	bool useGravity = true;

	bool isLinding_ = false;
	// 移動方向
	Vector3 direction_ = { 0.0f,0.0f,1.0f };
	// 回転補間速度
	float rotationSpeed = 0.1f;
};