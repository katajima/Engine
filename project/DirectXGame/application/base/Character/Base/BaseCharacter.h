#pragma once
#include "CharacterContext.h"
#include <DirectXGame/engine/Collider/ColliderData.h>
#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>
#include <DirectXGame/application/base/Attack/AttackData.h>
#include <DirectXGame/application/base/Attack/Input/AttackInputHandler.h>

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
		/// <summary>派生キャラクターのオブジェクト、能力、状態、依存システムを初期化する。</summary>
		/// <param name="inputSystem">操作入力。AI専用キャラクターではnullptrを許容する。</param>
		/// <param name="entity3DManager">キャラクターオブジェクトとコンポーネントの生成元。</param>
		/// <param name="globalVariables">キャラクター調整値の登録・保存先。</param>
		/// <param name="position">初期ワールド座標。</param>
		/// <param name="camera">描画に使用する非所有カメラ。</param>
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
		/// <summary>
		/// 攻撃(弱攻撃)
		/// </summary>
		void Attack() { RequestAttack(ActionInput::LightAttack); };
		/// 攻撃(強攻撃)
		void HeavyAttack() { RequestAttack(ActionInput::HeavyAttack); };
		/// <summary>
		/// 攻撃(スキル)
		/// </summary>
		void SkillAttack() { RequestAttack(ActionInput::Skill); };
		/// ステートマシーン初期化
		virtual void InitStateMachine() {};
		/// <summary>
		/// リロード
		/// </summary>
		virtual void Reload() {};
	public:
		/// <summary>プレイヤーまたは敵のキャラクター種別を設定する。</summary>
		/// <param name="type">設定するキャラクター種別。</param>
		void SetCharacterType(Type type);
		/// <summary>キャラクター種別を取得する。</summary>
		/// <returns>現在設定されている種別。</returns>
		Type GetCharacterType() const;
		/// <summary>キャラクターが生存しているか取得する。</summary>
		/// <returns>通常更新対象として生存している場合はtrue。</returns>
		bool GetAlive() const;
		/// <summary>キャラクターの生存状態を設定する。</summary>
		/// <param name="is">生存状態にする場合はtrue。</param>
		void SetAlive(bool is);
		/// <summary>現在HPを取得する。</summary>
		/// <returns>現在のHP。下限処理はパラメータコンポーネントに従う。</returns>
		float GetHP() const;
		/// <summary>現在HPへダメージを適用する。</summary>
		/// <param name="damage">減算するダメージ量。負数の扱いはパラメータ実装に従う。</param>
		void AddDamage(float damage);
		/// <summary>攻撃属性付きのダメージを適用し、死亡した場合はキル属性として記録する。</summary>
		/// <param name="damage">減算するダメージ量。</param>
		/// <param name="attribute">ダメージを発生させた攻撃属性。</param>
		void ApplyAttackDamage(float damage, AttackAttribute attribute);
		/// <summary>死亡原因になった攻撃属性を取得する。</summary>
		/// <returns>最後にHPを0以下にした攻撃属性。</returns>
		AttackAttribute GetFatalAttackAttribute() const { return fatalAttackAttribute_; }
		/// <summary>必殺技で倒されたか取得する。</summary>
		/// <returns>死亡原因が必殺属性ならtrue。</returns>
		bool WasKilledBySpecialAttack() const { return fatalAttackAttribute_ == AttackAttribute::Special; }
		/// <summary>
		/// 削除フラグ
		/// </summary>
		bool  GetDelete() const;
		/// <summary>
		/// 削除する
		/// </summary>
		void Delete();
		/// <summary>キャラクター固有の時間倍率を反映したフレーム時間を取得する。</summary>
		/// <returns>秒単位のフレーム時間。</returns>
		float GetTime();
		/// <summary>通常移動処理の許可状態を設定する。</summary>
		/// <param name="is">移動を許可する場合はtrue。</param>
		void IsMove(bool is);
		/// <summary>
		/// 移動可能か
		/// </summary>
		bool GetIsMove() const;
		/// <summary>基本能力パラメータを取得する。</summary>
		/// <returns>ParameterComponentが所有する変更可能ポインター。未初期化時はnullptr。</returns>
		BasicParameters* GetBasicParameters() const;
		/// <summary>
		/// 基本パラメータ
		/// </summary>
		BasicParameters* Parameters();
	public:
		/// <summary>
		/// タグ番号取得
		/// </summary>
		uint32_t GetTagNumber() const { return tagNumber_; }
		/// <summary>
		/// タグ番号設定
		/// </summary>
		void SetTagNumber(uint32_t tag) { tagNumber_ = tag; };
		/// <summary>
		/// 名前取得
		/// </summary>
		std::string GetName() const;
	public: // 取得系関数
		/// <summary>
		/// キャラクターステートマシーン取得
		/// </summary>
		CharacterStateMachine* GetCharacterStateMachine();
		/// <summary>
		/// 現在の状態取得
		/// </summary>
		CharacterMainState GetCurrentMainState() const;
		/// <summary>
		/// 過去のステート
		/// </summary>
		CharacterMainState GetPrevState() const;
		/// <summary>
		/// 必殺技取得
		/// </summary>
		BaseSpecial* GetSpecial();
		/// <summary>
		/// 武器取得
		/// </summary>
		BaseWeapon* GetWeapon();
		/// <summary>
		/// 弾の出現
		/// </summary>
		BulletSpawn* GetBulletSpawn();
		/// <summary>
		/// 死亡システム
		/// </summary>
		DeathSystem* GetDeathSystem();
		/// <summary>
		/// コライダーコンポーネント
		/// </summary>
		Engine::ColliderComponent* GetColliderComponent();
		/// <summary>
		/// オブジェクト3d取得
		/// </summary>
		ObjectComponent* GetObjectComponent();
		/// <summary>キャラクター本体のワールド変換を取得する。</summary>
		/// <returns>キャラクターの生存期間中有効な変更可能参照。</returns>
		Engine::WorldTransform& GetWorldTransform();
		/// <summary>キャラクター本体のワールド変換を読み取り専用で取得する。</summary>
		/// <returns>内部ObjectComponentが所有する非所有ポインター。</returns>
		const Engine::WorldTransform* GetConstWorldTransform() const;
		/// <summary>キャラクター本体の現在ワールド座標を取得する。</summary>
		/// <returns>ワールド空間の座標。</returns>
		Vector3 GetWorldPosition() const;
	public: // 貰いもの
		/// <summary>
		/// 弾マネージャ取得
		/// </summary>
		BulletManager* GetBulletManager() { return this->bulletManager; }
		/// <summary>
		/// カメラ管理クラス取得
		/// </summary>
		CameraManager* GetCameraManager() { return this->cameraManager; }
		/// <summary>
		/// ヒットボックス管理取得
		/// </summary>
		HitBox::System* GetHitBoxSystem() { return this->hitBoxSystem; }
		/// <summary>
		/// スペシャルポイント管理クラス取得
		/// </summary>
		SpecialPointManager* GetSpecialPointManager() { return this->specialPointManager; }
		/// <summary>
		/// 入力システム取得
		/// </summary>
		InputSystem* GetInputSystem() { return inputSystem; };
		/// <summary>キャラクター操作に使用する入力を設定する。</summary>
		/// <param name="inputSystem">非所有ポインター。AI操作へ切り替える場合はnullptrを許容する。</param>
		void SetInputSystem(InputSystem* inputSystem);
		/// <summary>
		/// カメラ取得
		/// </summary>
		Engine::Camera* GetCamera() const { return camera; }
		/// <summary>キャラクター描画と方向計算に使用するカメラを設定する。</summary>
		/// <param name="camera">非所有ポインター。キャラクター利用中は有効であること。</param>
		void SetCamera(Engine::Camera* camera);
		/// <summary>
		/// エフェクト設定
		/// </summary>
		void SetEffect(EffectSystem* effect) { this->effect = effect; }
		//
		EffectSystem* GetEffect() { return this->effect; }
		/// <summary>
		/// 弾マネージャーの設定
		/// </summary>
		void SetBulletManager(BulletManager* bulletManager) { this->bulletManager = bulletManager; };
		/// <summary>
		/// ヒットボックス管理の設定
		/// </summary>
		void SetHitBoxSystem(HitBox::System* hitBoxSystem) { this->hitBoxSystem = hitBoxSystem; }
		/// <summary>
		/// カメラ管理クラスの設定
		/// </summary>
		void SetCameraManager(CameraManager* cameraManager) { this->cameraManager = cameraManager; };
		/// <summary>
		/// スペシャルポイント管理クラス設定
		/// </summary>
		void SetSpecialPointManager(SpecialPointManager* specialPointManager) { this->specialPointManager = specialPointManager; }
	public:
		/// <summary>
		/// 移動コンポーネント取得
		/// </summary>
		MovementComponent* GetMoveComponent() { return moveComponent_.get(); }
		/// <summary>
		/// キャラクターパラメータコンポーネント取得
		/// </summary>
		ParameterComponent* GetCharacterParameterComponent() { return parameterComponent_.get(); }
		/// <summary>
		/// ヒットリアクションシステム取得
		/// </summary>
		HitMotionSystem* GetHitMotionSystem() { return hitMotionSystem_.get(); }
		/// <summary>
		/// 攻撃コントローラー取得
		/// </summary>
		AttackController* GetAttackController() { return attackController_.get(); }
	protected: // 保存機能
		/// <summary>
		/// 保存生成
		/// </summary>
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

		/// <summary>
		/// ベースの保存項目を追加
		/// </summary>
		void InitializeBaseAddItem();
		/// <summary>
		/// 更新保存項目
		/// </summary>
		void UpdateBaseGetValue();


		/// <summary>入力種別に対応するコンボ攻撃の開始または遷移を要求する。</summary>
		/// <param name="input">弱攻撃、強攻撃、スキルの入力種別。</param>
		/// <returns>攻撃要求が受理された場合はtrue。</returns>
		bool RequestAttack(ActionInput input);
		/// <summary>ダメージを無効化する状態か判定する。</summary>
		/// <param name="damage">今回適用しようとしているダメージ量。</param>
		/// <returns>ダメージを無視する場合はtrue。</returns>
		virtual bool ShouldIgnoreDamage(float damage) const { (void)damage; return false; }
		/// <summary>HPへダメージが実際に反映された後の通知を受け取る。</summary>
		/// <param name="damage">実際に適用されたダメージ量。</param>
		virtual void OnDamageApplied(float damage) { (void)damage; }
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
		// HPを0にした攻撃属性
		AttackAttribute fatalAttackAttribute_ = AttackAttribute::None;

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
		SpecialPointManager* specialPointManager = nullptr;
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
