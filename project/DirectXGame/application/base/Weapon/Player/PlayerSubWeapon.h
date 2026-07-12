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

	/// <summary>
	/// サブ武器に必要な入力、Entity、保存設定、カメラ参照を初期化します。
	/// </summary>
	/// <param name="inputSystem">武器入力を参照する入力システムです。所有権は受け取りません。</param>
	/// <param name="entityManager">武器モデルや当たり判定を登録するEntity管理です。所有権は受け取りません。</param>
	/// <param name="globalVariables">武器設定の読み書きに使うグローバル変数管理です。所有権は受け取りません。</param>
	/// <param name="position">初期配置位置です。ワールド座標で指定します。</param>
	/// <param name="camera">向き補正や描画で参照するカメラです。所有権は受け取りません。</param>
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) override;

	/// <summary>
	/// フレーム単位のサブ武器更新を行います。
	/// </summary>
	void Update() override;

	/// <summary>
	/// 経過時間を使って投擲、停止、回収状態を更新します。
	/// </summary>
	/// <param name="dt">前フレームからの経過時間です。単位は秒です。</param>
	void Update(float dt);

	/// <summary>
	/// サブ武器を指定位置から指定方向へ投擲します。
	/// </summary>
	/// <param name="startPosition">投擲開始位置です。ワールド座標で指定します。</param>
	/// <param name="direction">投擲方向です。呼び出し側で必要に応じて正規化してください。</param>
	void Throw(const Vector3& startPosition, const Vector3& direction);

	/// <summary>
	/// 現在位置で投擲物を停止状態にします。
	/// </summary>
	void StopAtCurrentPosition();

	/// <summary>
	/// 所有者の近くへサブ武器を回収します。
	/// </summary>
	void Recall();

	/// <summary>
	/// サブ武器の投擲データを設定します。
	/// </summary>
	/// <param name="data">速度、寿命、戻り時間、移動タイプなどの投擲調整データです。</param>
	void SetThrowData(const PlayerSubWeaponThrowData& data);

	/// <summary>
	/// 投擲中か確認します。
	/// </summary>
	/// <returns>Throw状態またはHold状態など、攻撃として外へ出ている場合はtrueです。</returns>
	bool IsThrowing() const;

	/// <summary>
	/// 投擲先で停止中か確認します。
	/// </summary>
	/// <returns>Hold状態の場合はtrue、それ以外はfalseです。</returns>
	bool IsHolding() const;

	/// <summary>
	/// サブ武器に紐づくエフェクトを描画します。
	/// </summary>
	void DrawEffect() override;

	/// <summary>
	/// サブ武器に紐づく2D要素を描画します。
	/// </summary>
	void Draw2D() override;

private:
	/// <summary>
	/// 待機中の位置を所有者基準のオフセットへ更新します。
	/// </summary>
	void UpdateIdle();

	/// <summary>
	/// 投擲中の位置、回転、自動回収判定を更新します。
	/// </summary>
	/// <param name="dt">前フレームからの経過時間です。単位は秒です。</param>
	void UpdateThrow(float dt);

	/// <summary>
	/// 停止中の保持時間や見た目の状態を更新します。
	/// </summary>
	/// <param name="dt">前フレームからの経過時間です。単位は秒です。</param>
	void UpdateHold(float dt);

	/// <summary>
	/// 所有者へ戻る補間位置を更新します。
	/// </summary>
	/// <param name="dt">前フレームからの経過時間です。単位は秒です。</param>
	void UpdateReturn(float dt);

	ThrowState throwState_ = ThrowState::kIdle;		// サブ武器の現在状態
	Vector3 throwStartPosition_{};					// 投擲開始位置
	Vector3 throwDirection_ = { 0.0f, 0.0f, 1.0f };	// 投擲方向
	Vector3 returnStartPosition_{};					// 戻り開始位置
	float throwTimer_ = 0.0f;						// 投擲の経過時間
	PlayerSubWeaponThrowData throwData_{};			// 投擲用の調整データ
};
