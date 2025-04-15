#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/base/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFanctions.h"


class Input;
class FollowCamera
{
public:
	
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(CameraCommon* cameraCommon);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	//追従対象をポインタで持つ
	void SetTarget(const Object3d* target) { target_ = target; };

	Camera& GetViewProjection() { return camera_; };


	//void SetCameraRotate(Vector3& rotate) { return camera_.transform_.rotate = rotate; }

private:
	//
	Input* input_;
	// ビュープロジェクション
	Camera camera_;

	//追従対象
	const Object3d* target_ = nullptr;

};

