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

		// 毎フレーム更新
		void Update() override;

		// エフェクト描画
		void DrawEffect() override {};
		// 描画2d
		void Draw2D() override {};
		// 移動
		void Move() override {};
		// ジャンプ
		void Jump() override {};
	private:
		// ステートマシーン初期化
		void InitStateMachine() override;
		// デバッグ用の表示と操作をまとめて更新
		void UpdateDebugImGui();
		// Damage状態へ入った瞬間に有効なデバッグ挙動を実行
		void ApplyDebugBehaviorOnHit();
		// 指定された地上/空中タイプに合わせて座標と物理速度を固定
		void ApplyDummyTypeLock();
		// ヒットリアクションやステートに合わせてダミーの色を更新
		void ApplyDebugStateColor();
		// ダミーを指定位置へ戻し、物理速度もリセット
		void ResetToDebugReturnPosition();
		// 現在のメインステートをデバッグ表示用の文字列へ変換
		const char* GetMainStateName(CharacterMainState state) const;
		// 現在のヒットリアクション状態をデバッグ表示用の文字列へ変換
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
