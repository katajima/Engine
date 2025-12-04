#pragma once
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"
#include "DirectXGame/application/base/Weapon/playerWeapon.h"
#include "DirectXGame/application/base/Special/RangeBombingSpecial.h"
#include "NormalPlayerState.h"

///< summary>
/// 自キャラ
///</summary>
class NormalPlayer : public BasePlayer{
public:


	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Input* input,Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables ,Vector3 position, Camera* camera) override;

	///< summary>
	/// 更新
	///</summary>
	void Update() override;

	/// <summary>
	/// エフェクトの描画
	/// </summary>
	void DrawEffect() override;

	/// <summary>
	/// 描画2d
	/// </summary>
	void Draw2D() override;

	// 移動処理
	void Move() override;

	// ジャンプ
	void Jump() override;

	// 攻撃
	void Attack() override;

	// プレイヤUI取得
	PlayerUI* GetPlayerUI()override { return ui_.get(); };

private:


	struct MoveGlovalData {
		// ジャンプ力
		float power_ = 800.0f;
		// 上昇時の重力係数
		float upGravity_ = 15.0f;
		// 落下時の重力係数
		float fallGravity_ = 30.0f;
		// 最小速度
		float minSpeed_ = 0.1f;
		// 最大速度
		float maxSpeed_ = 1.0f;
		// 加速度
		float speedAcceleration_ = 0.1f;
		// 回転速度
		float rotationSpeed_ = 0.1f;
		// スティックの角度によって速度が変化するか
		bool isStickToSpeed_ = true;
		// 空中での移動量制限を行うか
		bool isLimitAirSpeed_ = true;
		// 空中での移動変化量
		float airSpeedRate_ = 0.85f;


		// ダッシュ初速度
		float dashStartSpeed = 100.0f;
		// ダッシュ加速度
		float  dashAcceleration = 0.0f;
		// ダッシュ減衰度
		float  dashFriction = 10.0f;
		// 最大ダッシュ時間
		float  dashMaxTime = 0.15f;
		// ダッシュ中に重力適用させるかのフラグ
		bool isDashGravity = false;
	};
	MoveGlovalData moveGlovalData_;

private:
	void InitAttack();

	
private: 
	// ステートマシーン初期化
	void InitStateMachine() override;

	// 調整項目の適用
	void ApplyGlobalVariables();

	WorldTransform worldCollider_;
	std::unique_ptr<PlayerUI> ui_;			// プレイヤー用UI




	struct ComboGlovalData {
		float hitBoxWindowStart_ = 0.0f;	// ヒットボックスの生成タイミング時間
		float hitBoxLifeTime_ = 0.5f;		// ヒットボックスの生存時間

		float knockbackDuration_ = 0.1f;	// ノックバック持続時間
		float knockbackPower = 30.0f;		// ノックバックパワー
		float knockbackPowerY = 30.0f;		// ノックバックY方向パワー
		bool isVerticalBoost_ = false;		// Y方向にノックバックするか

		float damage = 20;					// ダメージ

		float stateInputStartTime = 0.1f;	// 入力受付時間
		float stateInputEndTime = 0.5f;		// 入力終了時間
		float stateEndTime = 0.5f;			// ステート終了時間
		float stateNextTime = 0.45f;		// ステート移行時間


		bool isGravity = true;				// 重力はあるか？


		float animationSpeed_ = 1.0f;		// アニメーションスピード


		Vector3 offsetPos_ = {};			// コライダー位置
		Vector3 colliderSize = { 1.0f,1.0f,1.0f };	// コライダーサイズ
	};
	ComboGlovalData data1_;
	ComboGlovalData data2_;
	ComboGlovalData data3_;

	ComboData comboData1{};
	ComboData comboData2{};
	ComboData comboData3{};


	//
	void ApplyGlobalComboData(const std::string& name, ComboGlovalData& data);

	void SetGlobalComboData(const std::string& name, ComboGlovalData& data);

	void ReloadComboData();

	//
	void SetData(ComboData& data ,const ComboGlovalData& gData);

	struct ProvisionalData {
		Vector3 size = { 1.25f,1.25f ,1.25f };// サイズ
		Vector3 scale_{ 3.0f,3.0f, 3.0f };// スケール

		float trailLifeTime = 0.5f;// トレイルの寿命
		Color color = { 1,1,1,0.25f };

		Vector3 obbColliderSize = { 0.5f,2.0f,1.0f };// OBBコライダーサイズ
		Vector3 obbCollider2Size = { 0.5f,5.5f,1.0f };// OBBコライダーサイズ

		Vector3 collider1Pos = { 0.0f,0.5f,0.0f };
		Vector3 collider2Pos = { 0,3.5f,0.5f };
	};
	ProvisionalData provisionalData_;

};