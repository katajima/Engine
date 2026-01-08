#pragma once
#include "MoveSystem.h"
#include "JumpSystem.h"
#include "DashSystem.h"
#include "AirMoveSystem.h"
#include "MovementRestrictions.h"
#include "MovementState.h"

// 前方宣言
namespace Engine {
	class GlobalVariables;
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
	void Initialize(Engine::GlobalVariables* globalVariables,ControlType type,const std::string& name = "");
	// 更新
	void Update(float dt, Engine::WorldTransform& object, Engine::RigidBodyComponent& rigid, Engine::Input* input);
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
	// 加速度
	Vector3& Acceleration() { return moveSystem_->Acceleration(); }
	// 向いている方向
	Vector3 GetDirection() const { return moveSystem_->GetDirection(); }
	

	// 速度設定
	void SetSpeed(float min, float max) { moveSystem_->SetSpeed(min, max); };
	// 移動タイプ設定
	void SetMoveType(MoveSystem::MoveType type) { moveSystem_->GetData().moveType = type;};
	// スティックによる移動速度固定設定
	void SetIsStickToSpeed(bool is) { moveSystem_->GetData().isStickToSpeed = is; };
	// 移動出来るか設定
	void SetCanMove(bool canMove) { moveSystem_->SetCanMove(canMove); }
	//
	void UseGlobal(bool is) { useGlobal_ = is; };
public: // ジャンプ系統

	// ジャンプ回数現象
	void DecrementJumpCount() { jumpSystem_->DecrementJumpCount(); }
	//	ジャンプ出来るか
	bool GetIsJump() const { return jumpSystem_->GetIsJump(); }
	// 着地状態か
	bool GetIsLanding() const { return jumpSystem_->GetIsLanding(); }
	// 最大ジャンプカウント設定
	void SetMaxJumpCount(int count) { jumpSystem_->SetMaxJumpCount(count); }

public:
	// 移動システム取得
	MoveSystem* GetMoveSystem() { return moveSystem_.get(); }
	// ジャンプシステム取得
	JumpSystem* GetJumpSystem() { return jumpSystem_.get(); }
	// ダッシュシステム取得
	DashSystem* GetDashSystem() { return dashSystem_.get(); }
public:
	// カメラ設定
	void SetCamera(Engine::Camera* camera) { moveSystem_->SetCamera(camera); }
private:
	std::unique_ptr<MoveSystem> moveSystem_ = nullptr; // 移動システム
	std::unique_ptr<JumpSystem> jumpSystem_ = nullptr; // ジャンプシステム
	std::unique_ptr<DashSystem> dashSystem_ = nullptr; // ダッシュシステム

	//
	std::unique_ptr<MovementRestrictions> movementRestrictions_ = nullptr; // 移動制限システム
	std::unique_ptr<MovementStateMachine> movementStateMachine_ = nullptr; // 移動状態マシン
	// 操作タイプ
	ControlType controlType_ = ControlType::Manual;	// 操作タイプ


	std::string name_ = "";

	bool useGlobal_ = true;
private:
	Engine::GlobalVariables* globalVariables = nullptr;

};




