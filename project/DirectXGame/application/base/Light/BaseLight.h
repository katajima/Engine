#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"


/// <summary>
/// ライト基底クラス
/// </summary>
class BaseLight
{
public:
	///< summary>
	/// 初期化
	///</summary>
	virtual void Initialize(Engine::Input* input, Engine::EntityManager* entity3DManager, Engine::GlobalVariables* globalVariables, Vector3 position) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;
protected:


};
