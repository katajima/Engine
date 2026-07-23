#pragma once
#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"



/// <summary>
/// SmallMeleeWeaponを管理・実装するクラス。
/// </summary>
class SmallMeleeWeapon : public MeleeWeapon
{
public:
	///< summary>
	/// 初期化
	///</summary>
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, 
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) override;

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

};

