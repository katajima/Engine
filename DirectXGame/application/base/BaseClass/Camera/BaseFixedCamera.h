#pragma once

#include"BaseCamera.h"

class BaseFixedCamera : public BaseCamera 
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


	virtual void Draw2D() = 0;




protected:
	Timer timer_;

};

