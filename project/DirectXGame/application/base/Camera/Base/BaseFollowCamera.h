#pragma once
#include "BaseCamera.h"

/// <summary>
/// フォローカメラ
/// </summary>
class BaseFollowCamera : public BaseCamera
{
public:
	///< summary>
	/// 初期化
	///</summary>
	virtual void Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables, Vector3 position) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;

	//追従対象をポインタで持つ
	void SetTarget(Engine::WorldTransform* target) { target_ = target; };
	
protected:
	//追従対象
	Engine::WorldTransform* target_ = nullptr;
	// ベースのオフセット（固定距離）
	Vector3 baseOffset = { 0.0f, 5.0f, -50.0f };
	// カメラ回転速度
	const float rotateSpeed = 0.03f;
};
