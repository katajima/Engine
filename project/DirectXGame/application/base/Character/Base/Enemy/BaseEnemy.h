#pragma once
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Character/Vision/VisionComponent.h"
#include"DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include"DirectXGame/application/base/Special/Base/BaseSpecial.h"
#include "DirectXGame/application/base/Character/Crowd/CrowdManager.h"
#include "EnemyData.h"

namespace Character {
	class BasePlayer;

	/// <summary>
	/// 敵クラス
	/// </summary>
	class BaseEnemy : public BaseCharacter {
	public:
		// 初期化
		virtual void Initialize(InputSystem* inputSystem, Engine::EntityManager* entity3DManager,
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) = 0;

		// 基盤となる初期化
		void BaseInitialize(InputSystem* inputSystem, Engine::EntityManager* entity3DManager,
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
		// ターゲット取得
		const BaseCharacter* GetTarget();

		void SetTargetCharacters(BaseCharacter* target);


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
		// ターゲット方向取得
		Vector3 TargetDirection();
		

		// ロックオンされているか
		bool GetIsLockOn() const { return isLockOn_; }
		//　ロックオンされているか設定
		void SetIsLockOn(bool isLockOn) { isLockOn_ = isLockOn; }
	protected:
		// スプライト初期化
		void Initialize2D();

	private:

		void InitializeBaseEnemyAddItem() {
			AddItem("後退スピード", globalData_.retreatSpeed);
			AddItem("攻撃猶予時間", globalData_.attackTimer);
			AddItem("攻撃猶予範囲", globalData_.attackStartRadius);
			AddItem("後退開始範囲", globalData_.startRetreatingRadius);



			globalData_.retreatSpeed = GetValue<float>("後退スピード");
			globalData_.attackTimer = GetValue<float>("攻撃猶予時間");
			globalData_.attackStartRadius = GetValue<float>("攻撃猶予範囲");
			globalData_.startRetreatingRadius = GetValue<float>("後退開始範囲");

		}
		// 更新保存項目
		void UpdateBaseEnemyGetValue() {
			globalData_.retreatSpeed = GetValue<float>("後退スピード");
			globalData_.attackTimer = GetValue<float>("攻撃猶予時間");
			globalData_.attackStartRadius = GetValue<float>("攻撃猶予範囲");
			globalData_.startRetreatingRadius = GetValue<float>("後退開始範囲");
		}

		//
		void InitShadowObjectComponent(const std::string& charaName);

	protected:
		EnemyType type_ = EnemyType::kMediumMelee; // 敵の種類
		uint32_t id_ = 0; // ID
	protected:

	protected: //2D
		std::unique_ptr<Engine::Sprite> icon_lockOn;
	protected:
		std::unique_ptr <VisionComponent> visionComponent_;			// 視界
		std::unique_ptr<Engine::EffectComponent> effectComponent_ = nullptr;
		bool isLockOn_ = false; // ロックオンされているか

		// グローバルデータ
		EnemyGlobalData globalData_;
	protected:
		Engine::WorldTransform worldEffect_;
	};
}