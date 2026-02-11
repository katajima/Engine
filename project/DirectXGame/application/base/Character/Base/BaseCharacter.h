#pragma once
#include"CharacterData.h"
#include"DirectXGame/application/base/Move/MoveComponent.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "DirectXGame/application/base/State/CharacterStateMachine.h"
#include <DirectXGame/application/base/Attack/Response/Response.h>
#include <DirectXGame/application/base/Attack/AttackController.h>
#include "DirectXGame/application/base/Bullet/base/BulletSpawn.h" 

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
		virtual void Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
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
		// ワールド変換取得
		Engine::WorldTransform& GetWorldTransform() { return objectComponent_->GetWorldTransform(); }
		// 削除フラグ
		bool  GetDelete() const { return objectComponent_->GetObjectStateFlags().isDeleted; };
		// 削除する
		void Delete() { objectComponent_->GetObjectStateFlags().isDeleted = true; };
		// 時間
		float GetTime() { return objectComponent_->GetTime(); }

	public: // 貰いもの
		// インプット取得
		Engine::Input* GetInput() { return this->input; };
		// 弾マネージャ取得
		BulletManager* GetBulletManager() { return this->bulletManager; }
		// カメラ管理クラス取得
		CameraManager* GetCameraManager() { return this->cameraManager; }
		// スペシャルポイント管理クラス取得
		SpecalPointManager* GetSpecalPointManager() { return this->specalPointManager; }

		//エフェクト設定
		void SetEffect(Effect* effect) { this->effect = effect; }
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

	protected:


	public:
		// 速度
		Vector3& Velocity() { return moveComponent_->Velocity(); }
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
		std::unique_ptr<ObjectComponent> objectComponent_;			// オブジェクトコンポーネント
		std::unique_ptr<BaseSpecial> special_;						// スペシャル攻撃
		std::unique_ptr<BaseWeapon> weapon_;						// 武器
		std::unique_ptr<AttackInputHandler> attackInputHandler_;		// 攻撃入力系クラス

		std::unique_ptr<CharacterStateMachine> stateMachine_;		// キャラクターの状態管理
		std::unique_ptr<MovementComponent> moveComponent_;			// 移動コンポーネント

		std::unique_ptr<AttackController> attackController_;		// 攻撃制御クラス

		std::unique_ptr<ResponseSystem> responseSystem_;			// 攻撃応答システムクラス


		std::unique_ptr<BulletSpawn> bulletSpawn_;					// 弾出現
	protected:
		// キャラクターパラメータコンポーネント
		std::unique_ptr <ParameterComponent> parameterComponent_;
	protected: // 貰いもの(アプリケーション層)
		Effect* effect = nullptr;							// エフェクト
		BulletManager* bulletManager = nullptr;			// 弾管理
		CameraManager* cameraManager = nullptr;			// カメラ管理クラス
		SpecalPointManager* specalPointManager = nullptr;	// 必殺技ポイント管理クラス
	protected: // 貰ってくるもの(エンジン層)
		Engine::Entity3DManager* entity3DManager = nullptr;	// 3Dエンティティマネージャー
		Engine::Entity2DManager* entity2DManager = nullptr;	// 2Dエンティティマネージャー
		Engine::GlobalVariables* globalVariables = nullptr;	// グローバル変数
		Engine::Camera* camera = nullptr;						// カメラ
		Engine::Input* input = nullptr;						// 入力(使わないならnullptr)
		Engine::Audio* audio = nullptr;
	};
}