#pragma once
#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"

/// <summary>
/// プレイヤーサブ武器
/// </summary>
class PlayerSubWeapon : public MeleeWeapon {
public:
	/// <summary>
	/// サブ武器の行動状態
	/// </summary>
	enum class ThrowState {
		kIdle,		// 待機中
		kThrow,		// 投擲中
		kReturn,	// 戻り中
	};

	///< summary>
	/// 初期化
	///</summary>
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) override;

	///< summary>
	/// 更新
	///</summary>
	void Update() override;

	/// <summary>
	/// 経過時間を使った更新
	/// </summary>
	void Update(float dt);

	/// <summary>
	/// サブ武器を投擲する
	/// </summary>
	void Throw(const Vector3& startPosition, const Vector3& direction);

	/// <summary>
	/// 投擲中か
	/// </summary>
	bool IsThrowing() const;

	/// <summary>
	/// エフェクトの描画
	/// </summary>
	void DrawEffect() override;

	/// <summary>
	/// 描画2d
	/// </summary>
	void Draw2D() override;

private:
	/// <summary>
	/// 待機中の位置を更新する
	/// </summary>
	void UpdateIdle();

	/// <summary>
	/// 投擲中の位置を更新する
	/// </summary>
	void UpdateThrow(float dt);

	/// <summary>
	/// 戻り中の位置を更新する
	/// </summary>
	void UpdateReturn(float dt);

	ThrowState throwState_ = ThrowState::kIdle;		// サブ武器の現在状態
	Vector3 idleOffset_ = { 0.6f, 1.1f, 0.2f };		// プレイヤー基準の待機位置
	Vector3 throwDirection_ = { 0.0f, 0.0f, 1.0f };	// 投擲方向
	Vector3 returnStartPosition_{};					// 戻り開始位置
	float throwTimer_ = 0.0f;						// 投擲の経過時間
	float throwSpeed_ = 28.0f;						// 投擲速度
	float throwLifeTime_ = 0.35f;					// 投擲状態の継続時間
	float returnTime_ = 0.18f;						// 戻りにかける時間
	float spinSpeed_ = 18.0f;						// 投擲中の回転速度
};
