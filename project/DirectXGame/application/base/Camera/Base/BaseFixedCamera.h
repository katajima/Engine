#pragma once

#include"BaseCamera.h"


/// <summary>
/// 固定カメラクラス
/// </summary>
class BaseFixedCamera : public BaseCamera 
{
public:
	///< summary>
	/// 初期化
	///</summary>
	virtual void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables, Vector3 position) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;

protected:
	Timer timer_;

};

