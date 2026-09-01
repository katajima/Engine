#pragma once
#include <DirectXGame/engine/Move/MovementRestrictions.h>
#include "MovementSystem.h"
#include "DirectXGame/application/base/Character/Move/Base/LocomotionCoordinator.h"
#include "DirectXGame/application/base/Character/Move/Move/MoveSystem.h"
#include "DirectXGame/application/base/Character/Move/Jump/JumpSystem.h"
#include "MoveRequestSystem.h"
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

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Character::BaseCharacter* owner ,InputSystem* input,Engine::GlobalVariables* globalVariables,ControlType type,const std::string& name = "");
	/// <summary>
	/// 更新
	/// </summary>
	void Update(Engine::WorldTransform& object, Engine::RigidBodyComponent& rigid, const Character::CharacterContext& ctx);
public:
	/// <summary>
	/// 保存項目の追加
	/// </summary>
	void ApplyGlobalData(const std::string& name);
	/// <summary>
	/// 保存項目の適応
	/// </summary>
	void SetGlobalData(const std::string& name);
public:
	/// <summary>
	/// 操作タイプ取得
	/// </summary>
	void SetControlType(ControlType type) { controlType_ = type; }
public: // 移動系統
	/// <summary>
	/// 速度取得
	/// </summary>
	Vector3 GetVelocity() const { return moveSystem_->GetVelocity(); }
	/// <summary>
	/// 向いている方向
	/// </summary>
	Vector3 GetDirection() const { return movementSystem_->GetDirection(); }
	

	/// <summary>
	/// 速度設定
	/// </summary>
	void SetSpeed(float min, float max) { moveSystem_->SetSpeed(min, max); };
	/// <summary>
	/// 移動タイプ設定
	/// </summary>
	void SetMoveType(MoveType type) { moveSystem_->Data().moveType = type;};
	/// <summary>
	/// スティックによる移動速度固定設定
	/// </summary>
	void SetIsStickToSpeed(bool is) { moveSystem_->Data().isStickToSpeed = is; };
	/// <summary>
	/// 保存項目を使うか
	/// </summary>
	void UseGlobal(bool is) { useGlobal_ = is; };
public: // ジャンプ系統

	/// <summary>
	/// ジャンプ回数現象
	/// </summary>
	void DecrementJumpCount() { jumpSystem_->DecrementJumpCount(); }
	/// <summary>
	/// ジャンプ出来るか
	/// </summary>
	bool GetIsJump() const { return jumpSystem_->GetIsJump(); }
	/// <summary>
	/// 残りジャンプ回数取得
	/// </summary>
	int GetJumpCount() const { return jumpSystem_->GetJumpCount(); }
	/// <summary>
	/// 着地状態か
	/// </summary>
	bool GetIsLanding() const { return movementSystem_->IsOnGround(); }
	/// <summary>
	/// 最大ジャンプカウント設定
	/// </summary>
	void SetMaxJumpCount(int count) { jumpSystem_->SetMaxJumpCount(count); }
	// 
	void SetAttackingGravity(float gravity) { attackingGravity = gravity; }
public:
	/// <summary>
	/// 移動システム取得
	/// </summary>
	MoveSystem* GetMoveSystem() { return moveSystem_.get(); }
	/// <summary>
	/// ジャンプシステム取得
	/// </summary>
	JumpSystem* GetJumpSystem() { return jumpSystem_.get(); }
	/// <summary>
	/// 攻撃移動システム取得
	/// </summary>
	MoveRequestSystem* GetMoveRequestSystem() { return moveRequestSystem_.get(); }
	/// <summary>
	/// 移動システム取得
	/// </summary>
	MovementSystem* GetMovementSystem() { return movementSystem_.get(); }
public:
	/// <summary>
	/// カメラ設定
	/// </summary>
	void SetCamera(Engine::Camera* camera) { this->camera = camera; }
private:
	// 移動システム
	std::unique_ptr<MoveSystem> moveSystem_ = nullptr;
	// ジャンプシステム
	std::unique_ptr<JumpSystem> jumpSystem_ = nullptr;
	// 移動リクエストシステム
	std::unique_ptr<MoveRequestSystem> moveRequestSystem_ = nullptr;
private:
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




