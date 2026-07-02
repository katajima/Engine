#pragma once
#include "CharacterContext.h"
#include <DirectXGame/engine/Collider/ColliderData.h>
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>


class HitResponse;
class HitMotionSystem;
class AttackController;
class BulletSpawn;
class DeathSystem;
class MoveComponent;
class ParameterComponent;
class InputSystem;

class BaseSpecial;
class BaseWeapon;

namespace HitBox {
	class System;
}

namespace  Engine {
	class Audio;
	class EntityManager;
	class ColliderComponent;
	class WorldTransform;
}

namespace Character {

	class CharacterStateMachine;

	/// <summary>
	/// キャラクター基底クラス
	/// </summary>
	class BaseCharacter : public IHitReceiver
	{
	public:
		BaseCharacter();
		virtual ~BaseCharacter(); // = default をヘッダーに書かない
		/// 初期化
		virtual void Initialize(InputSystem* inputSystem, Engine::EntityManager* entity3DManager,
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) = 0;
		/// 更新
		virtual void Update() = 0;
		/// エフェクトの描画
		virtual void DrawEffect() = 0;
		/// 2d描画
		virtual void Draw2D() = 0;
		/// 移動
		virtual void Move() = 0;
		/// ジャンプ
		virtual void Jump() = 0;
		/// 回避
		virtual void Dodge() {};
		/// 回避成功通知
		virtual void OnDodgeSuccess() {};
		/// 回避成功後のコンボ受付中か
		virtual bool IsDodgeSuccessComboWindow() const { return false; };
		/// 攻撃(弱攻撃)
		virtual void Attack() = 0;
		/// 攻撃(強攻撃)
		virtual void HeavyAttack() {};
		/// 攻撃(スキル攻撃)
		virtual void SkillAttack() {};
		/// ステートマシーン初期化
		virtual void InitStateMachine() {};
		// リロード
		virtual void Reload() {};
	public:
		// キャラクタータイプ設定
		void SetCharacterType(Type type);
		// キャラクター取得
		Type GetCharacterType() const;
		// キャラクターの生存状態を取得
		bool GetAlive() const;
		// キャラクターの生存状態を取得
		void SetAlive(bool is);
		// HP取得
		float GetHP() const;
		// ダメージ
		void AddDamage(float damage);
		// 削除フラグ
		bool  GetDelete() const;
		// 削除する
		void Delete();
		// 時間
		float GetTime();
		// 移動出来るか設定
		void IsMove(bool is);
		// 移動可能か
		bool GetIsMove() const;
		// パラメータ取得
		BasicParameters* GetBasicParameters() const;
		// 基本パラメータ
		BasicParameters* Parameters();
	public:
		// タグ番号取得
		uint32_t GetTagNumber() const { return tagNumber_; }
		// タグ番号設定
		void SetTagNumber(uint32_t tag) { tagNumber_ = tag; };
		// 名前取得
		std::string GetName() const;
	public: // 取得系関数
		// キャラクターステートマシーン取得
		CharacterStateMachine* GetCharacterStateMachine();
		// 現在の状態取得
		CharacterMainState GetCurrentMainState() const;
		// 過去のステート
		CharacterMainState GetPrevState() const;
		// 必殺技取得
		BaseSpecial* GetSpecial();
		// 武器取得
		BaseWeapon* GetWeapon();
		// 弾の出現
		BulletSpawn* GetBulletSpawn();
		// 死亡システム
		DeathSystem* GetDeathSystem();
		// コライダーコンポーネント
		Engine::ColliderComponent* GetColliderComponent();
		// オブジェクト3d取得
		ObjectComponent* GetObjectComponent();
		// ワールド変換取得
		Engine::WorldTransform& GetWorldTransform();
		// ワールド変換取得
		const Engine::WorldTransform* GetConstWorldTransform() const;
		// ワールド座標取得
		Vector3 GetWorldPosition() const;
	public: // 貰いもの
		// 弾マネージャ取得
		BulletManager* GetBulletManager() { return this->bulletManager; }
		// カメラ管理クラス取得
		CameraManager* GetCameraManager() { return this->cameraManager; }
		// ヒットボックス管理取得
		HitBox::System* GetHitBoxSystem() { return this->hitBoxSystem; }
		// スペシャルポイント管理クラス取得
		SpecalPointManager* GetSpecalPointManager() { return this->specalPointManager; }
		// 入力システム取得
		InputSystem* GetInputSystem() { return inputSystem; };
		// 入力システム設定
		void SetInputSystem(InputSystem* inputSystem);
		// カメラ取得
		Engine::Camera* GetCamera() const { return camera; }
		// カメラ設定
		void SetCamera(Engine::Camera* camera);
		//エフェクト設定
		void SetEffect(EffectSystem* effect) { this->effect = effect; }
		//
		EffectSystem* GetEffect() { return this->effect; }
		// 弾マネージャーの設定
		void SetBulletManager(BulletManager* bulletManager) { this->bulletManager = bulletManager; };
		// ヒットボックス管理の設定
		void SetHitBoxSystem(HitBox::System* hitBoxSystem) { this->hitBoxSystem = hitBoxSystem; }
		// カメラ管理クラスの設定
		void SetCameraManager(CameraManager* cameraManager) { this->cameraManager = cameraManager; };
		// スペシャルポイント管理クラス設定
		void SetSpecalPointManager(SpecalPointManager* specalPointManager) { this->specalPointManager = specalPointManager; }
	public:
		// 移動コンポーネント取得
		MovementComponent* GetMoveComponent() { return moveComponent_.get(); }
		// キャラクターパラメータコンポーネント取得
		ParameterComponent* GetCharacterParameterComponent() { return parameterComponent_.get(); }
		// ヒットリアクションシステム取得
		HitMotionSystem* GetHitMotionSystem() { return hitMotionSystem_.get(); }
		// 攻撃コントローラー取得
		AttackController* GetAttackController() { return attackController_.get(); }
	protected: // 保存機能
		// 保存生成
		void CreateGroup(const std::string name);
		// 保存するもの追加
		template<typename T>
		void AddItem(const std::string itemName, T& item) {
			globalVariables->AddItem(GetName(), itemName, item);
		}
		// 値取得
		template<typename T>
		T GetValue(const std::string itemName) {
			return globalVariables->GetValue<T>(GetName(), itemName);
		}

		// ベースの保存項目を追加
		void InitializeBaseAddItem();
		// 更新保存項目
		void UpdateBaseGetValue();
	protected:
		// オブジェクトコンポーネント
		std::unique_ptr<ObjectComponent> objectComponent_ = nullptr;
		// スペシャル攻撃
		std::unique_ptr<BaseSpecial> special_ = nullptr;
		// 武器
		std::unique_ptr<BaseWeapon> weapon_ = nullptr;
		// キャラクターの状態管理
		std::unique_ptr<CharacterStateMachine> stateMachine_ = nullptr;
		// 移動コンポーネント
		std::unique_ptr<MovementComponent> moveComponent_ = nullptr;
		// 攻撃制御クラス
		std::unique_ptr<AttackController> attackController_ = nullptr;
		// ヒットリアクションシステムクラス
		std::unique_ptr<HitMotionSystem> hitMotionSystem_ = nullptr;
		// HitResponse
		std::unique_ptr<HitResponse> hitResponse_ = nullptr;
		// 死亡システム
		std::unique_ptr<DeathSystem> deathSystem_ = nullptr;
		// 弾出現
		std::unique_ptr<BulletSpawn> bulletSpawn_ = nullptr;
	protected:
		// キャラクターパラメータコンポーネント
		std::unique_ptr <ParameterComponent> parameterComponent_ = nullptr;
		// キャラクタータグ
		uint32_t tagNumber_ = 0;
		// コンテキストシステム
		std::unique_ptr<CharacterContextSystem> contextSystem_ = nullptr;
		//
		bool isMove = true;

		std::unique_ptr < Engine::WorldTransform> worldCollider_= nullptr;
	protected: // 貰いもの(アプリケーション層)
		// エフェクト
		EffectSystem* effect = nullptr;
		// 弾管理
		BulletManager* bulletManager = nullptr;
		// ヒットボックス管理
		HitBox::System* hitBoxSystem = nullptr;
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
