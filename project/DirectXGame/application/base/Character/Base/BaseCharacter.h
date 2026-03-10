#pragma once
#include"CharacterData.h"
#include"DirectXGame/application/base/Move/Base/MoveComponent.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/application/base/State/CharacterStateMachine.h"
#include <DirectXGame/application/base/Attack/Response/Response.h>
#include <DirectXGame/application/base/Attack/AttackController.h>
#include "DirectXGame/application/base/Bullet/base/BulletSpawn.h" 
#include "CharacterContext.h"

namespace Character {

	/// <summary>
	/// キャラクター基底クラス
	/// </summary>
	class BaseCharacter : public IHitReceiver
	{
	public:
		~BaseCharacter() = default;


		///< summary>
		/// 初期化
		///</summary>
		virtual void Initialize(InputSystem* inputSystem, Engine::EntityManager* entity3DManager,
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) = 0;

		///< summary>
		/// 更新
		///</summary>
		virtual void Update() = 0;

		/// <summary>
		/// エフェクトの描画
		/// </summary>
		virtual void DrawEffect() = 0;

		/// <summary>
		/// 2d描画
		/// </summary>
		virtual void Draw2D() = 0;

		/// <summary>
		/// 移動
		/// </summary>
		virtual void Move() = 0;

		/// <summary>
		/// ジャンプ
		/// </summary>
		virtual void Jump() = 0;

		/// <summary>
		/// 攻撃(弱攻撃)
		/// </summary>
		virtual void Attack() = 0;

		/// <summary>
		/// 攻撃(強攻撃)
		/// </summary>
		virtual void HeavyAttack() {};

		/// <summary>
		/// 攻撃(スキル攻撃)
		/// </summary>
		virtual void SkillAttack() {};


		/// <summary>
		/// ステートマシーン初期化
		/// </summary>
		virtual void InitStateMachine() {};

	public:
		// キャラクタータイプ設定
		void SetCharacterType(Type type) { parameterComponent_->characterType_ = type; }

		// ダメージ
		void AddDamage(float damage) {
			HP() -= damage;		// HPをダメージ分減算
			if (GetHP() <= 0) {
				HP() = 0;
				objectComponent_->GetObjectStateFlags().isAlive = false; // 敵が死亡
			}
		}

	public: // 取得系関数

		// キャラクターステートマシーン取得
		CharacterStateMachine* GetCharacterStateMachine() { return stateMachine_.get(); }
		// 現在の状態取得
		CharacterMainState GetCurrentMainState() const { return stateMachine_->GetCurrentMainState(); }
		// 必殺技取得
		BaseSpecial* GetSpecial() { return special_.get(); }
		// 武器取得
		BaseWeapon* GetWeapon() { return weapon_.get(); }
		// 弾の出現
		BulletSpawn* GetBulletSpawn() { return bulletSpawn_.get(); };

		// キャラクターの生存状態を取得
		bool GetAlive() const { return objectComponent_->GetObjectStateFlags().isAlive; };
		// キャラクターの生存状態を取得
		void SetAlive(bool is) { objectComponent_->GetObjectStateFlags().isAlive = is; };

		// HP取得
		float GetHP() const { return parameterComponent_->parameters_->HP.value; }
		// キャラクター取得
		Type GetCharacterType() const { return parameterComponent_->characterType_; }
		// コライダーコンポーネント
		Engine::ColliderComponent* GetColliderComponent() { return objectComponent_->GetColliderComponent(); };
		// オブジェクト3d取得
		ObjectComponent* GetObjectComponent() { return objectComponent_.get(); }
		// オブジェクト3d取得
		ObjectComponent* GetObjectComponentShadow() { return objectComponentShadow_.get(); }

		// ワールド変換取得
		Engine::WorldTransform& GetWorldTransform() { return objectComponent_->GetWorldTransform(); }
		// ワールド座標取得
		Vector3 GetWorldPosition() const { return objectComponent_->GetWorldTransform().GetWorldPosition(); }
		// 削除フラグ
		bool  GetDelete() const { return objectComponent_->GetObjectStateFlags().isDeleted; };
		// 削除する
		void Delete() { objectComponent_->GetObjectStateFlags().isDeleted = true; };
		// 時間
		float GetTime() { return objectComponent_->GetTime(); }

	public: // 取得
		Engine::Camera* GetCamera() const { return camera; }
		// パラメータ取得
		BasicParameters* GetBasicParameters() const { return parameterComponent_->parameters_.get(); }
	public:
		// タグ番号取得
		uint32_t GetTagNumber() const { return tagNumber_; }
		// タグ番号設定
		void SetTagNumber(uint32_t tag) { tagNumber_ = tag; };
	public: // 貰いもの
		// 弾マネージャ取得
		BulletManager* GetBulletManager() { return this->bulletManager; }
		// カメラ管理クラス取得
		CameraManager* GetCameraManager() { return this->cameraManager; }
		// スペシャルポイント管理クラス取得
		SpecalPointManager* GetSpecalPointManager() { return this->specalPointManager; }
		// 入力システム取得
		InputSystem* GetInputSystem() { return inputSystem; };

		//エフェクト設定
		void SetEffect(EffectSystem* effect) { this->effect = effect; }
		// 弾マネージャーの設定
		void SetBulletManager(BulletManager* bulletManager) { this->bulletManager = bulletManager; };
		// カメラ管理クラスの設定
		void SetCameraManager(CameraManager* cameraManager) { this->cameraManager = cameraManager; };
		// スペシャルポイント管理クラス設定
		void SetSpecalPointManager(SpecalPointManager* specalPointManager) { this->specalPointManager = specalPointManager; }
	protected: // 取得系関数(変更可能)

		// 基本パラメータ
		BasicParameters* Parameters() { return parameterComponent_->parameters_.get(); }
		// HP
		float& HP() { return parameterComponent_->parameters_->HP.value; }

	protected: // 保存機能

		// 保存生成
		void CreateGroup(const std::string name) {
			objectComponent_->SetName(name);
			globalVariables->CreateGroup(name);
		}

		// 保存するもの追加
		template<typename T>
		void AddItem(const std::string itemName, T& item) {
			globalVariables->AddItem(objectComponent_->GetName(), itemName, item);
		}

		// 値取得
		template<typename T>
		T GetValue(const std::string itemName) {
			return globalVariables->GetValue<T>(objectComponent_->GetName(), itemName);
		}

		// ベースの保存項目を追加
		void InitializeBaseAddItem() {
			AddItem("speed", parameterComponent_->parameters_->speed);
			AddItem("HP", parameterComponent_->parameters_->HP.value);
			AddItem("MaxHP", parameterComponent_->parameters_->HP.maxValue);
			AddItem("MP", parameterComponent_->parameters_->MP.value);
			AddItem("MaxMP", parameterComponent_->parameters_->MP.maxValue);
			AddItem("stamina", parameterComponent_->parameters_->stamina.value);
			AddItem("MaxStamina", parameterComponent_->parameters_->stamina.maxValue);
			AddItem("jampPower", parameterComponent_->parameters_->jampPower);



			parameterComponent_->parameters_->speed = GetValue<float>("speed");
			parameterComponent_->parameters_->HP.value = GetValue<float>("HP");
			parameterComponent_->parameters_->HP.maxValue = GetValue<float>("MaxHP");
			parameterComponent_->parameters_->MP.value = GetValue<float>("MP");
			parameterComponent_->parameters_->MP.maxValue = GetValue<float>("MaxMP");
			parameterComponent_->parameters_->stamina.value = GetValue<float>("stamina");
			parameterComponent_->parameters_->stamina.maxValue = GetValue<float>("MaxStamina");
			parameterComponent_->parameters_->jampPower = GetValue<float>("jampPower");
		}
		// 更新保存項目
		void UpdateBaseGetValue() {
			parameterComponent_->parameters_->speed = GetValue<float>("speed");
			parameterComponent_->parameters_->jampPower = GetValue<float>("jampPower");
		}
	public:
		// 速度取得
		Vector3 GetVelocity() const { return moveComponent_->GetVelocity(); }
		// 移動コンポーネント取得
		MovementComponent* GetMoveComponent() { return moveComponent_.get(); }

	public:
		// キャラクターパラメータコンポーネント取得
		ParameterComponent* GetCharacterParameterComponent() { return parameterComponent_.get(); }
		// 攻撃応答システム取得
		ResponseSystem* GetResponseSystem() { return responseSystem_.get(); }
		// 攻撃コントローラー取得
		AttackController* GetAttackController() { return attackController_.get(); }

	protected:
		// オブジェクトコンポーネント
		std::unique_ptr<ObjectComponent> objectComponent_;
		// 影用オブジェクトコンポーネント
		std::unique_ptr<ObjectComponent> objectComponentShadow_ = nullptr;
		// スペシャル攻撃
		std::unique_ptr<BaseSpecial> special_;
		// 武器
		std::unique_ptr<BaseWeapon> weapon_;
		// 攻撃入力系クラス
		std::unique_ptr<AttackInputHandler> attackInputHandler_;
		// キャラクターの状態管理
		std::unique_ptr<CharacterStateMachine> stateMachine_;
		// 移動コンポーネント
		std::unique_ptr<MovementComponent> moveComponent_;
		// 攻撃制御クラス
		std::unique_ptr<AttackController> attackController_;
		// 攻撃応答システムクラス
		std::unique_ptr<ResponseSystem> responseSystem_;
		// 弾出現
		std::unique_ptr<BulletSpawn> bulletSpawn_;
	protected:
		// キャラクターパラメータコンポーネント
		std::unique_ptr <ParameterComponent> parameterComponent_;
		// キャラクタータグ
		uint32_t tagNumber_ = 0;
		// コンテキストシステム
		std::unique_ptr<CharacterContextSystem> contextSystem_ = nullptr;
	protected: // 貰いもの(アプリケーション層)
		// エフェクト
		EffectSystem* effect = nullptr;							
		// 弾管理
		BulletManager* bulletManager = nullptr;					
		// カメラ管理クラス
		CameraManager* cameraManager = nullptr;					
		// 必殺技ポイント管理クラス
		SpecalPointManager* specalPointManager = nullptr;			
	protected: // 貰ってくるもの(エンジン層)
		// 3Dエンティティマネージャー
		Engine::EntityManager* entityManager = nullptr;		
		// グローバル変数
		Engine::GlobalVariables* globalVariables = nullptr;	
		// カメラ
		Engine::Camera* camera = nullptr;	
		// 音
		Engine::Audio* audio = nullptr;
		// 入力(使わないならnullptr)
		InputSystem* inputSystem = nullptr;					
	};
}