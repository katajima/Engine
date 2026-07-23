#pragma once
#include"DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"
#include "DummyEnemyState.h"

namespace Character {
/// <summary>
/// DummyEnemyを管理・実装するクラス。
/// </summary>
	class DummyEnemy : public BaseEnemy {
	public:
		// ダミーを地上扱いにするか空中扱いにするか
		enum class DummyType {
			Ground,	// 地上ダミー
			Air,	// 空中ダミー
		};

		DummyEnemy() {

		}

		///< summary>
		/// 初期化
		///</summary>
		void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) override;

		/// <summary>
		/// 毎フレーム更新
		/// </summary>
		void Update() override;

		/// <summary>
		/// エフェクト描画
		/// </summary>
		void DrawEffect() override {};
		/// <summary>
		/// 描画2d
		/// </summary>
		void Draw2D() override {};
		/// <summary>
		/// 移動
		/// </summary>
		void Move() override {};
		/// <summary>
		/// ジャンプ
		/// </summary>
		void Jump() override {};
	private:
		/// <summary>
		/// ステートマシーン初期化
		/// </summary>
		void InitStateMachine() override;
		/// <summary>
		/// デバッグ用の表示と操作をまとめて更新
		/// </summary>
		void UpdateDebugImGui();
		/// <summary>
		/// Damage状態へ入った瞬間に有効なデバッグ挙動を実行
		/// </summary>
		void ApplyDebugBehaviorOnHit();
		/// <summary>
		/// 指定された地上/空中タイプに合わせて座標と物理速度を固定
		/// </summary>
		void ApplyDummyTypeLock();
		/// <summary>
		/// ヒットリアクションやステートに合わせてダミーの色を更新
		/// </summary>
		void ApplyDebugStateColor();
		/// <summary>
		/// ダミーを指定位置へ戻し、物理速度もリセット
		/// </summary>
		void ResetToDebugReturnPosition();
		/// <summary>
		/// 現在のメインステートをデバッグ表示用の文字列へ変換
		/// </summary>
		const char* GetMainStateName(CharacterMainState state) const;
		/// <summary>
		/// 現在のヒットリアクション状態をデバッグ表示用の文字列へ変換
		/// </summary>
		const char* GetHitMotionStateName(HitMotionSystem::HitMotionState state) const;

	private:
		float hp = 100.0f;						// ダミーの最大HP
		bool showDebugWindow_ = true;			// DummyEnemy専用ImGuiを表示するか
		bool completeStop_ = false;				// ヒットリアクション後も指定位置に固定するか
		bool returnOnHit_ = false;				// 被弾した瞬間に指定位置へ戻すか
		bool recoverImmediatelyOnHit_ = false;	// 被弾した瞬間にDamage状態とリアクションを終了するか
		bool lockDummyType_ = false;				// 地上/空中タイプに合わせて毎フレーム位置を固定するか
		bool useDebugStateColor_ = true;			// 状態確認用にモデルカラーを変更するか
		bool wasDamageState_ = false;			// 前フレームにDamage状態だったか
		DummyType dummyType_ = DummyType::Ground;// 検証用のダミータイプ
		Vector3 returnPosition_{};				// 被弾時や完全停止時に戻す座標
		float groundHeight_ = -3.0f;				// 地上固定時のY座標
		float airHeight_ = 2.0f;					// 空中固定時のY座標
	};

}
