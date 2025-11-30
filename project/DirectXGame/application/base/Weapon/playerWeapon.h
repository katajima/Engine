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

private: // 一旦

	struct ProvisionalData {
		Vector3 size = { 1.25f,1.25f ,1.25f };// サイズ
		Vector3 scale_{ 3.0f,3.0f, 3.0f };// スケール

		float trailLifeTime = 0.5f;// トレイルの寿命
		Color color = { 1,1,1,0.25f };

		Vector3 obbColliderSize = { 0.5f,2.0f,1.0f };// OBBコライダーサイズ
		Vector3 obbCollider2Size = { 0.5f,5.5f,1.0f };// OBBコライダーサイズ


		Vector3 collider1Pos = {0.0f,0.5f,0.0f};
		Vector3 collider2Pos = {0,3.5f,0.5f};
	};
	ProvisionalData provisionalData_;
};

