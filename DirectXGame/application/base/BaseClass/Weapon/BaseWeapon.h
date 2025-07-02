#pragma once
#include "DirectXGame/application/base/BaseClass/Character/BaseCharacter.h"

class BaseWeapon : public BaseObject
{
public:
	///< summary>
	/// 初期化
	///</summary>
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;

	/// <summary>
	/// エフェクトの描画
	/// </summary>
	virtual void DrawEffect() = 0;

	/// <summary>
	/// 2d描画
	/// </summary>
	virtual void Draw2D() = 0;

public:
	// タグによるコライダーの有効・無効を設定
	void SetIsCollider(CollisionTag tag ,bool is) { objectBase_->GetColliderComponent()->SetEnableByTag(tag, is); };

	virtual void SetCharacter(BaseCharacter* character) = 0;


};