#pragma once
#include "BaseCamera.h"


class BaseRailCamera : public BaseCamera {
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
	std::vector<Vector3> controlPoint;	// 制御点
	float currentDistance = 0;			// 現在の位置(0.0f~1.0f)
	float moveSpeed = 0.01f;			// 進むスピード
	bool isLooping = false;				// ループさせるか
	bool autoMove = false;				// 自動で進むか

};