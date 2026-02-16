#pragma once
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Character/Vision/VisionComponent.h"
#include"DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include"DirectXGame/application/base/Special/Base/BaseSpecial.h"
#include "DirectXGame/application/base/Character/Crowd/CrowdManager.h"

//前方宣言

class Entity3DManager;
class Entity2DManager;

namespace Character {
	class BasePlayer;

	/// <summary>
	/// 敵クラス
	/// </summary>
	class BaseEnemy : public BaseCharacter {
	public:
		// 初期化
		virtual void Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) = 0;

		// 基盤となる初期化
		void BaseInitialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera,
			const std::string& modelName, const std::string& charaName, float colliderRadius = 1.0f);

		// 毎フレーム更新
		virtual void Update() = 0;

		// 基盤の更新
		void BaseUpdate();


		// 描画エフェクト
		virtual void DrawEffect() = 0;
		// 描画2d
		virtual void Draw2D() = 0;
		// 移動
		virtual void Move() = 0;
		// ジャンプ
		virtual void Jump() = 0;
		// 攻撃
		virtual void Attack() = 0;
		// プレイヤー設定
		void SetTarget(BasePlayer* target) { target_ = target; }// プレイヤ設定

		BasePlayer* GetTarget() { return target_; } // ターゲット取得

	public:
		// パーティクル発生
		virtual void Emit() = 0;
		// ID設定
		void SetID(uint32_t id) { id_ = id; }
		// 敵タイプ取得
		EnemyType GetType() const { return type_; };
		// 敵タイプ設定
		void SetType(EnemyType type) { type_ = type; };

		// 対象の位置(プレイヤー)
		Vector3 GetTargetPos();
		// 対象との距離(プレイヤー)
		float GetTargetDistance();
		// 移動方向＋移動
		void DirectionMoveVelocity(float speed);
		// 移動向き
		void DirectionMove(float speed);
		// ターゲット方向取得
		Vector3 TargetDirection();
		//
		void TargetMove(const Vector3 velocity);


		// ロックオンされているか
		bool GetIsLockOn() const { return isLockOn_; }
		//　ロックオンされているか設定
		void SetIsLockOn(bool isLockOn) { isLockOn_ = isLockOn; }
	protected:
		// スプライト初期化
		void Initialize2D();

	private:
		//
		void InitShadowObjectComponent(const std::string& charaName);

	protected:
		EnemyType type_ = EnemyType::kNormal; // 敵の種類
		uint32_t id_ = 0; // ID

		AgentState agentState_ = AgentState::Approach;
	protected:

	protected: //2D
		std::unique_ptr<Engine::Sprite> icon_lockOn;
		std::unique_ptr<Engine::Sprite> hpBer_;
		std::unique_ptr<Engine::Sprite> backHpBer_;
	protected:
		BasePlayer* target_;
		std::unique_ptr <VisionComponent> visionComponent_;			// 視界
		std::unique_ptr<Engine::EffectComponent> effectComponent_ = nullptr;
		bool isLockOn_ = false; // ロックオンされているか


	protected:
		Engine::WorldTransform worldEffect_;
		DebugTimer debugTimer_;

		// 影用オブジェクトコンポーネント
		std::unique_ptr<ObjectComponent> objectComponentShadow_ = nullptr;

	};
}