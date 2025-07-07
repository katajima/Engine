#pragma once
#include "DirectXGame/application/base/BaseClass/Weapon/Weapons.h"

class BasePlayer;
class Player;
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


	void Draw2D() override;

private:
	WorldTransform colliderWorld_;
	uint32_t weaponColliderId_ = 0;

	WorldTransform colliderWorld2_;
	uint32_t weaponColliderId2_ = 0;

	//BasePlayer* basePlayer_;
};

