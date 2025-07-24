#pragma once
#include "BaseWeapon.h"


// 剣武器のクラス
class SwordWeapon : public MellWeapon
{
public:
	/// 初期化
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;
	/// 更新
	virtual void Update() = 0;
	/// エフェクトの描画
	virtual void DrawEffect() = 0;
	/// 2d描画
	virtual void Draw2D() = 0;

	// 攻撃各コンボによる初期化
	//virtual void AttackTypeInit(int comboIndex) = 0;

	// 攻撃更新
	//virtual void AttackUpdate(float deltaTime, WorldTransform& worldTransform) = 0;
protected:

};

// ハンマー武器のクラス
class HammerWeapon : public MellWeapon
{
public:
	/// 初期化
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;
	/// 更新
	virtual void Update() = 0;
	/// エフェクトの描画
	virtual void DrawEffect() = 0;
	/// 2d描画
	virtual void Draw2D() = 0;

	// 攻撃各コンボによる初期化
	//virtual void AttackTypeInit(int comboIndex) = 0;

	// 攻撃更新
	//virtual void AttackUpdate(float deltaTime, WorldTransform& worldTransform) = 0;
protected:

};

// 槍武器のクラス
class SpearWeapon : public MellWeapon
{
public:
	/// 初期化
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;
	/// 更新
	virtual void Update() = 0;
	/// エフェクトの描画
	virtual void DrawEffect() = 0;
	/// 2d描画
	virtual void Draw2D() = 0;

	// 攻撃各コンボによる初期化
	//virtual void AttackTypeInit(int comboIndex) = 0;

	// 攻撃更新
	//virtual void AttackUpdate(float deltaTime, WorldTransform& worldTransform) = 0;
protected:

};





