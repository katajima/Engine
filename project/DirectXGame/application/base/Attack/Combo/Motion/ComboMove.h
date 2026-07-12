#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
#include <DirectXGame/engine/struct/Vector3.h>


// 前方宣言
namespace Engine {
	class AnimationComponent;	// アニメーション
	class RigidBodyComponent;	// リジットボディー
	class Camera;
	class WorldTransform;
}

class MovementComponent;	// 移動関係
namespace Character {
	class BaseCharacter;		// キャラクター
}

class BaseWeapon;			// 武器
class JumpSystem;			// ジャンプシステム
class MoveRequestSystem;	// 移動リクエストシステム
class LockOnSystem;			// ロックオンシステム
struct MoveRequest;			// 移動リクエスト

namespace Combo {

	class ComboMove {
	public:
		/// <summary>
		/// コンボ移動に必要な所有者、Transform、移動システムなどの参照を取得して開始状態を作ります。
		/// </summary>
		/// <param name="owner">コンボを実行するキャラクターです。所有権は受け取りません。</param>
		/// <param name="ctx">移動、カメラ、ロックオンなど外部システム参照をまとめたコンテキストです。</param>
		void Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx);

		/// <summary>
		/// コンボデータに従い、時間経過に応じた移動要求や慣性を更新します。
		/// </summary>
		/// <param name="ctx">接地状態や移動システムなどを参照するコンテキストです。</param>
		/// <param name="timer">コンボステート開始からの経過時間です。単位は秒です。</param>
		void Update(const Character::CharacterContext& ctx, float timer);

		/// <summary>
		/// コンボ移動終了時の一時状態を解除します。
		/// </summary>
		/// <param name="owner">コンボ移動を終了するキャラクターです。</param>
		void Exit(Character::BaseCharacter* owner);

	public:
		/// <summary>
		/// 現在コンボ移動を実行できる状態か取得します。
		/// </summary>
		/// <returns>移動要求を出せる場合はtrue、停止中や無効状態ならfalseです。</returns>
		bool IsMove() const { return isMove_; }

		/// <summary>
		/// コンボ移動の調整データを取得します。
		/// </summary>
		/// <returns>編集可能な移動データ参照を返します。</returns>
		GlobalMove& GetData() { return data_; }

		/// <summary>
		/// 移動の基準方向を取得します。
		/// </summary>
		/// <returns>ワールド空間での方向ベクトルを返します。</returns>
		Vector3 GetDirection() const { return direction_; }

		/// <summary>
		/// 移動の基準方向を設定します。
		/// </summary>
		/// <param name="dire">設定する方向ベクトルです。呼び出し側で必要に応じて正規化してください。</param>
		void SetDirection(const Vector3& dire) { direction_ = dire; }

		/// <summary>
		/// ターゲット移動で使う目標位置を取得します。
		/// </summary>
		/// <returns>現在解決されているターゲット位置を返します。</returns>
		Vector3 GetTargetPosition() const { return targetPos_; }

		/// <summary>
		/// 現在追従・接近対象として使うTransformを取得します。
		/// </summary>
		/// <returns>ターゲットがある場合はTransform、見つからない場合はnullptrです。</returns>
		const Engine::WorldTransform* GetTarget();
	private:
		/// <summary>
		/// 移動タイプに応じて通常移動、ターゲット移動、瞬間移動などを処理します。
		/// </summary>
		/// <param name="timer">コンボステート開始からの経過時間です。単位は秒です。</param>
		/// <param name="dt">前フレームからの経過時間です。単位は秒です。</param>
		void MoveTypeProcess(float timer, float dt);

		/// <summary>
		/// 移動タイプに合わせて移動方向や目標位置を更新します。
		/// </summary>
		void MoveTypeDirectionProcess();

		/// <summary>
		/// ローカル移動ベクトルを基準方向やカメラ方向へ変換し、最終移動方向を作ります。
		/// </summary>
		/// <returns>ワールド空間での移動方向を返します。</returns>
		Vector3 BuildMoveDirection() const;

		/// <summary>
		/// ターゲットへ接近する際、停止半径を考慮した到達目標位置を計算します。
		/// </summary>
		/// <returns>停止半径を反映したワールド座標を返します。</returns>
		Vector3 BuildTargetMoveGoal() const;

		/// <summary>
		/// ターゲット移動用の移動要求を作成して適用します。
		/// </summary>
		/// <param name="request">書き込み先の移動要求です。</param>
		/// <param name="dt">前フレームからの経過時間です。単位は秒です。</param>
		/// <returns>ターゲット移動を適用した場合はtrue、適用しなかった場合はfalseです。</returns>
		bool ApplyTargetMove(MoveRequest& request, float dt);

		/// <summary>
		/// 移動時間内の進行度から速度倍率を計算します。
		/// </summary>
		/// <param name="timer">コンボステート開始からの経過時間です。単位は秒です。</param>
		/// <returns>速度に掛ける倍率を返します。</returns>
		float CalculateMoveCurveScale(float timer) const;

		/// <summary>
		/// 攻撃開始前から引き継いだ移動慣性を、今フレームの移動要求へ追加します。
		/// </summary>
		/// <param name="dt">前フレームからの経過時間です。単位は秒です。</param>
		void RequestMoveInertia(float dt);

		/// <summary>
		/// 接地状態に応じた摩擦または空気抵抗で、保持している移動慣性を減衰します。
		/// </summary>
		/// <param name="dt">前フレームからの経過時間です。単位は秒です。</param>
		void ApplyMoveInertiaResistance(float dt);

		/// <summary>
		/// ターゲット有無に応じて使用する移動速度を取得します。
		/// </summary>
		/// <returns>有効な移動速度ベクトルを返します。</returns>
		Vector3 GetActiveMoveSpeed() const;

		/// <summary>
		/// ターゲット有無に応じて使用するローカル移動方向を取得します。
		/// </summary>
		/// <returns>ローカル空間での移動方向を返します。</returns>
		Vector3 GetActiveLocalMoveVector() const;

		/// <summary>
		/// ターゲット有無に応じてローカル移動方向を正規化するか取得します。
		/// </summary>
		/// <returns>正規化する場合はtrue、そのまま使う場合はfalseです。</returns>
		bool GetActiveNormalizeLocalMove() const;

		/// <summary>
		/// ターゲット有無に応じて使用するターゲット移動タイプを取得します。
		/// </summary>
		/// <returns>有効なターゲット移動タイプを返します。</returns>
		TargetMoveType GetActiveTargetMoveType() const;

		/// <summary>
		/// ターゲット有無に応じて使用する接近停止半径を取得します。
		/// </summary>
		/// <returns>停止半径をワールド単位で返します。</returns>
		float GetActiveMoveTargetRadius() const;
	private:
		// 移動
		MovementComponent* moveComponent = nullptr;
		// ワールドトランスフォーム
		Engine::WorldTransform* worldTransform = nullptr;
		// ワールドトランスフォーム（ターゲット）
		const Engine::WorldTransform* targetWorldTransform = nullptr;


		// ロックオンシステム
		LockOnSystem* lockOnSystem = nullptr;
		// ターゲット
		const Character::BaseCharacter* traget = nullptr;
		// 攻撃移動システム
		MoveRequestSystem* moveRequestSystem = nullptr;
		// カメラ基準移動を計算するために参照するカメラ（非所有）
		const Engine::Camera* camera = nullptr;
	private:
		// データ
		GlobalMove data_;
		// 方向
		Vector3 direction_ = {};
		// 最終移動方向
		Vector3 moveDirection_ = {};
		// 攻撃開始直前から引き継ぐ水平移動速度
		Vector3 moveInertiaVelocity_ = {};
		// 移動出来るか
		bool isMove_ = true;
		// ターゲット瞬間移動を実行したか
		bool isTargetTeleported_ = false;
	private:
		// ターゲット位置
		Vector3 targetPos_ = {};
		// スティック方向
		Vector2 stickDirection_ = {};
	};
}
