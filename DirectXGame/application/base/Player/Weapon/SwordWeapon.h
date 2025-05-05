#pragma once

#include "DirectXGame/application/base/Attack/Weapon/BaseWeapon.h"

class BasePlayer;
class Player;
class SwordWeapon : public BaseWeapon {
public:
	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Entity3DManager* entity3DManager, Camera* camera) override;

	///< summary>
	/// 更新
	///</summary>
	void Update() override;

	///< summary>
	/// 描画
	///</summary>
	void Draw() override;


protected:
	void UsePlayer() override;

	void UseEnemy() override;

};
