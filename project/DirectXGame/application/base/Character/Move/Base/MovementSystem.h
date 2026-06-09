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
	// 着地状態か？
	bool IsOnGround() const{ return isLinding_; }
	// 現在キャラクターが向いている移動方向を取得
	Vector3 GetDirection() const { return direction_;}
	// 重力処理を使うか設定
	void SetUseGravity(bool isUse) { useGravity = isUse; }
	// 重力速度をリセットする。外部から着地や演出で速度を消す時に使う
	void ResetGravityVelocity();
private:
	// 地面判定、重力ON/OFF、剛体積分をまとめて処理する
	void GravityProess(const Character::CharacterContext& cxt,Engine::WorldTransform& world,Engine::RigidBodyComponent& rigid);

	// ステートによって重力スケールを切り替える
	void StateGravityProcess(const Character::CharacterContext& cxt, Engine::WorldTransform& world, Engine::RigidBodyComponent& rigid);
	
	// 現在の移動方向へ向くようにY軸回転を補間する
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
