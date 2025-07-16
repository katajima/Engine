#pragma once
#include "BaseCamera.h"

class BaseFollowCamera : public BaseCamera
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


	//追従対象をポインタで持つ
	void SetTarget(BaseObject* target) { target_ = target; };
	
protected:
	//追従対象
	BaseObject* target_ = nullptr;
	// ベースのオフセット（固定距離）
	Vector3 baseOffset = { 0.0f, 5.0f, -50.0f };
	// カメラ回転速度
	const float rotateSpeed = 0.03f;
};
