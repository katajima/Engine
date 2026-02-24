#pragma once
#include "MovementRestrictions.h"
#include "MovementSystem.h"
#include "DirectXGame/application/base/Move/Base/LocomotionCoordinator.h"
#include "DirectXGame/application/base/Move/Move/MoveSystem.h"
#include "DirectXGame/application/base/Move/Jump/JumpSystem.h"
#include "DirectXGame/application/base/Move/Dash/DashSystem.h"

// 前方宣言
namespace Engine {
	class GlobalVariables;
}


namespace Character {
	class BaseCharacter;
}

/// <summary>
/// 移動コンポーネント
/// </summary>
class MovementComponent
{
public:
	// 操作タイプ
	enum class ControlType
	{
		Auto,			// 自動
		Manual,			// 手動
	};

	// 初期化
	void Initialize(Character::BaseCharacter* owner ,InputSystem* input,Engine::GlobalVariables* globalVariables,ControlType type,const std::string& name = "");
	// 更新
	void Update(float dt, Engine::WorldTransform& object, Engine::RigidBodyComponent& rigid, InputSystem* input);
public:
	// 保存項目の追加
	void ApplyGlobalData(const std::string& name);
	// 保存項目の適応
	void SetGlobalData(const std::string& name);
public:
	// 操作タイプ取得
	void SetControlType(ControlType type) { controlType_ = type; }
public: // 移動系統

	// 速度取得
	Vector3 GetVelocity() const { return moveSystem_->GetVelocity(); }
	// 速度
	Vector3& Velocity() { return moveSystem_->Velocity(); }
	// 向いている方向
	Vector3 GetDirection() const { return movementSystem_->GetDirection(); }
	

	// 速度設定
	void SetSpeed(float min, float max) { moveSystem_->SetSpeed(min, max); };
	// 移動タイプ設定
	void SetMoveType(MoveType type) { moveSystem_->GetData().moveType = type;};
	// スティックによる移動速度固定設定
	void SetIsStickToSpeed(bool is) { moveSystem_->GetData().isStickToSpeed = is; };
	// 移動出来るか設定
	void SetCanMove(bool canMove) { moveSystem_->SetCanMove(canMove); }
	// 保存項目を使うか
	void UseGlobal(bool is) { useGlobal_ = is; };
public: // ジャンプ系統

	// ジャンプ回数現象
	void DecrementJumpCount() { jumpSystem_->DecrementJumpCount(); }
	//	ジャンプ出来るか
	bool GetIsJump() const { return jumpSystem_->GetIsJump(); }
	// 着地状態か
	bool GetIsLanding() const { return movementSystem_->IsOnGround(); }
	// 最大ジャンプカウント設定
	void SetMaxJumpCount(int count) { jumpSystem_->SetMaxJumpCount(count); }
	// 
	void SetAttackingGravity(float gravity) { attackingGravity = gravity; }
public:
	// 移動システム取得
	MoveSystem* GetMoveSystem() { return moveSystem_.get(); }
	// ジャンプシステム取得
	JumpSystem* GetJumpSystem() { return jumpSystem_.get(); }
	// ダッシュシステム取得
	DashSystem* GetDashSystem() { return dashSystem_.get(); }
	// 移動システム取得
	MovementSystem* GetMovementSystem() { return movementSystem_.get(); }
public:
	// カメラ設定
	void SetCamera(Engine::Camera* camera) { this->camera = camera; }
private:
	// 移動システム
	std::unique_ptr<MoveSystem> moveSystem_ = nullptr;
	// ジャンプシステム
	std::unique_ptr<JumpSystem> jumpSystem_ = nullptr;
	// ダッシュシステム
	std::unique_ptr<DashSystem> dashSystem_ = nullptr; 
	// 移動制限システム
	std::unique_ptr<MovementRestrictions> movementRestrictions_ = nullptr; 
	// 行動リクエストを元に行動を集約→選択するクラス
	std::unique_ptr<LocomotionCoordinator> locomotionCoordinator_ = nullptr;
	// 行動リクエストを元に移動速度→トランスフォーム更新を行うクラス
	std::unique_ptr<MovementSystem> movementSystem_ = nullptr;

private:
	// 操作タイプ
	ControlType controlType_ = ControlType::Manual;
	// 名前
	std::string name_ = "";
	// 保存項目を使うか
	bool useGlobal_ = true;
private:
	// 保存項目
	Engine::GlobalVariables* globalVariables = nullptr;
	// 所有者
	Character::BaseCharacter* owner = nullptr;
	// カメラ
	const Engine::Camera* camera = nullptr;
	// 攻撃中の重力係数
	float attackingGravity = 1.0f;
};




