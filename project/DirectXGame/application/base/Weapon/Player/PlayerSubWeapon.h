#pragma once
#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"

/// <summary>
/// プレイヤーサブ武器の投擲調整データ
/// </summary>
struct PlayerSubWeaponThrowData {
	enum class MoveType {
		kStraight,			// 直進
		kStopOnHit,			// ヒット時に停止
		kTeleportToTarget,	// ターゲット位置へ瞬間移動
		kLerpToTarget,		// ターゲット位置へ補間移動
	};

	Vector3 idleOffset = { 0.6f, 1.1f, 0.2f };		// プレイヤー基準の待機位置
	Vector3 targetPosition{};						// 移動タイプ用の目標位置
	float throwSpeed = 28.0f;						// 投擲速度
	float throwLifeTime = 0.35f;					// 投擲状態の継続時間
	float returnTime = 0.18f;						// 戻りにかける時間
	float lerpTime = 0.25f;							// 目標位置へ補間移動する時間
	float spinSpeed = 18.0f;						// 投擲中の回転速度
	bool autoReturn = true;							// 投擲時間後に自動で戻るか
	bool alignToDirection = true;					// 投擲方向へ向きを合わせるか
	bool useSpin = true;							// 投擲中に回転させるか
	MoveType moveType = MoveType::kStraight;		// 投擲物の移動タイプ
	Vector3 rotateOffset = { 1.5708f,0.0f,3.1416f };	// 投擲姿勢の回転オフセット
};

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
		kHold,		// 投擲位置で停止中
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
	/// 現在位置で停止する
	/// </summary>
	void StopAtCurrentPosition();

	/// <summary>
	/// 所有者の近くへ回収する
	/// </summary>
	void Recall();

	/// <summary>
	/// サブ武器の投擲データを設定する
	/// </summary>
	void SetThrowData(const PlayerSubWeaponThrowData& data);

	/// <summary>
	/// 投擲中か
	/// </summary>
	bool IsThrowing() const;

	/// <summary>
	/// 停止中か
	/// </summary>
	bool IsHolding() const;

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
	/// 停止中の位置を更新する
	/// </summary>
	void UpdateHold(float dt);

	/// <summary>
	/// 戻り中の位置を更新する
	/// </summary>
	void UpdateReturn(float dt);

	ThrowState throwState_ = ThrowState::kIdle;		// サブ武器の現在状態
	Vector3 throwStartPosition_{};					// 投擲開始位置
	Vector3 throwDirection_ = { 0.0f, 0.0f, 1.0f };	// 投擲方向
	Vector3 returnStartPosition_{};					// 戻り開始位置
	float throwTimer_ = 0.0f;						// 投擲の経過時間
	PlayerSubWeaponThrowData throwData_{};			// 投擲用の調整データ
};
