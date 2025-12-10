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

	// 攻撃(弱攻撃)
	void Attack() override;

	/// 攻撃(強攻撃)
	void HeavyAttack() override;



	// プレイヤUI取得
	PlayerUI* GetPlayerUI()override { return ui_.get(); };

private:
	void InitAttack();

	
private: 
	// ステートマシーン初期化
	void InitStateMachine() override;

	// 調整項目の適用
	void ApplyGlobalVariables();

	WorldTransform worldCollider_;
	std::unique_ptr<PlayerUI> ui_;			// プレイヤー用UI


	ComboGlovalData data1_;
	ComboGlovalData data2_;
	ComboGlovalData data3_;
	ComboGlovalData data4_;

	ComboData comboData1{};
	ComboData comboData2{};
	ComboData comboData3{};
	ComboData comboData4{};


	void ReloadComboData();

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