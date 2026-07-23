#pragma once
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"
#include "DirectXGame/engine/Move/RigidBodyComponent.h"
#include "LocomotionContext.h"
#include "DirectXGame/application/base/Character/Base/CharacterContext.h"

/// <summary>
/// 移動リクエストを元に最終速度を反映し、重力と向きの更新まで行う移動処理クラス。
/// </summary>
class MovementSystem {
public:

	/// <summary>
	/// 移動システムの初期化。現状は初期値のみで動くため空実装。
	/// </summary>
	void Initialize();


	/// <summary>
	/// キャラクター状態と移動命令を元に、座標・回転・重力を更新する。
	/// </summary>
	/// <param name="ctx">キャラクターの状態情報</param>
	/// <param name="cmd">今フレームの移動命令</param>
	/// <param name="world">更新対象のワールド座標</param>
	/// <param name="rigid">重力と速度を持つ剛体情報</param>
	void Update(const Character::CharacterContext& ctx,const MoveCommand& cmd,Engine::WorldTransform& world,Engine::RigidBodyComponent& rigid);

public:
	/// <summary>
	/// 着地状態か？
	/// </summary>
	bool IsOnGround() const{ return isLinding_; }
	/// <summary>
	/// 現在キャラクターが向いている移動方向を取得
	/// </summary>
	Vector3 GetDirection() const { return direction_;}
	/// <summary>
	/// 重力処理を使うか設定
	/// </summary>
	void SetUseGravity(bool isUse) { useGravity = isUse; }
	/// <summary>
	/// 重力速度をリセットする。外部から着地や演出で速度を消す時に使う
	/// </summary>
	void ResetGravityVelocity();
private:
	/// <summary>
	/// 地面判定、重力ON/OFF、剛体積分をまとめて処理する
	/// </summary>
	void GravityProess(const Character::CharacterContext& cxt,Engine::WorldTransform& world,Engine::RigidBodyComponent& rigid);

	/// <summary>
	/// ステートによって重力スケールを切り替える
	/// </summary>
	void StateGravityProcess(const Character::CharacterContext& cxt, Engine::WorldTransform& world, Engine::RigidBodyComponent& rigid);
	
	/// <summary>
	/// 現在の移動方向へ向くようにY軸回転を補間する
	/// </summary>
	void RotateProcess(const Character::CharacterContext& cxt, Engine::WorldTransform& world);
private:
	// 着地判定に使う地面の高さ
	float groundHeight_ = -3.0f;
	bool useGravity = true;	// この移動システムで重力を制御するか

	bool isLinding_ = false;	// 今フレーム地面に接しているか
	// 移動方向
	Vector3 direction_ = { 0.0f,0.0f,1.0f };
	// 回転補間速度
	float rotationSpeed = 0.1f;
};
