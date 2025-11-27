#pragma once
#include "DirectXGame/application/base/Weapon/Base/MellWeapons.h"


// 前方宣言
class BasePlayer;
class NormalPlayer;


/// <summary>
///  プレイヤー武器クラス
/// </summary>
class PlayerWeapon : public SwordWeapon
{
public:

	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) override;

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

private:
	WorldTransform colliderWorld_;
	uint32_t weaponColliderId_ = 0;

	WorldTransform colliderWorld2_;
	uint32_t weaponColliderId2_ = 0;

	std::shared_ptr<ComboNodeState> attack1;
	std::shared_ptr<ComboNodeState> attack2;
	std::shared_ptr<ComboNodeState> attack3;
	std::shared_ptr<ComboNodeState> heavy1;

private: // 一旦

	struct ProvisionalData {
		Vector3 size = { 1.25f,1.25f ,1.25f };// サイズ
		Vector3 scale_{ 3.0f,3.0f, 3.0f };// スケール

		float trailLifeTime = 0.5f;// トレイルの寿命
		Color color = { 1,1,1,0.25f };

		Vector3 obbColliderSize = { 0.5f,2.0f,1.0f };// OBBコライダーサイズ
		Vector3 obbCollider2Size = { 0.5f,5.5f,1.0f };// OBBコライダーサイズ

		float damage1 = 20;
		float damage2 = 21;
		float damage3 = 35;

		float knockbackPower1 = 30.0f;
		float knockbackPowerY1 = 30.0f;

		float knockbackPower2 = 130.0f;
		float knockbackPowerY2 = 90.0f;


		Vector3 collider1Pos = {0.0f,2.5f,0.5f};
		Vector3 collider2Pos = {0,2.0f,0};


	};
	ProvisionalData provisionalData_;
};

