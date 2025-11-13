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
};

