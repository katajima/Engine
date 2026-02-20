#pragma once
#include "BaseCamera.h"


/// <summary>
/// レールカメラクラス
/// </summary>
class BaseRailCamera : public BaseCamera {
public:
	///< summary>
	/// 初期化
	///</summary>
	virtual void Initialize(InputSystem* inputSystem, Entity3DManager* entity3DManager,  GlobalVariables* globalVariables, Vector3 position) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;
protected:
	std::vector<Vector3> controlPoint;	// 制御点
	float currentDistance = 0;			// 現在の位置(0.0f~1.0f)
	float moveSpeed = 0.01f;			// 進むスピード
	bool isLooping = false;				// ループさせるか
	bool autoMove = false;				// 自動で進むか

};