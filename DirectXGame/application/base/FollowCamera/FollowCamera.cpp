#include "FollowCamera.h"

#include"DirectXGame/engine/input/Input.h"
#include "DirectXGame/engine/Camera/CameraCommon.h"

void FollowCamera::Initialize(CameraCommon* cameraCommon)
{
	input_ = cameraCommon->GetInput();

	//camera_ = Camera::GetInstance();
	camera_.Initialize(cameraCommon);
	camera_.farClip_ = 5000.0f;
	camera_.transform_.rotate.x = DegreesToRadians(90);
	camera_.transform_.rotate.x = DegreesToRadians(20);

}

void FollowCamera::Update()
{
	//追従対象がいれば
	if (target_) {
		
		const float kRotateSpeed = 0.000003f * 10000;

		if (input_->IsControllerConnected())
		{
			camera_.transform_.rotate.y += input_->GetGamePadRightStick().x * kRotateSpeed;
			camera_.transform_.rotate.x += input_->GetGamePadRightStick().y * kRotateSpeed;

			if (camera_.transform_.rotate.x >= DegreesToRadians(60)) {
				camera_.transform_.rotate.x = DegreesToRadians(60);
			}
			
			if (camera_.transform_.rotate.x <= DegreesToRadians(0)) {
				camera_.transform_.rotate.x = DegreesToRadians(0);
			}


		}
		else {
			if (input_->IsPushKey(DIK_LEFT)) {
				camera_.transform_.rotate.y -= 0.01f;
			}
			if (input_->IsPushKey(DIK_RIGHT)) {
				camera_.transform_.rotate.y += 0.01f;
			}

			if (camera_.transform_.rotate.x >= DegreesToRadians(60)) {
				camera_.transform_.rotate.x = DegreesToRadians(60);
			}

			if (camera_.transform_.rotate.x <= DegreesToRadians(0)) {
				camera_.transform_.rotate.x = DegreesToRadians(0);
			}
		}


		// 追従対象からカメラまでのオフセット
		Vector3 offset = { 0.0f, 5.0f, -50.0f };

		Matrix4x4 matrix = MakeRotateYMatrix(camera_.transform_.rotate.y);
		Matrix4x4 matrix2 = MakeRotateXMatrix(camera_.transform_.rotate.x);

		// オフセットをカメラの回転に合わせて回転させる
		offset = TransformNormal(offset, matrix2 * matrix);

		// 座標をコピーしてオフセット分ずらす
		camera_.transform_.translate = Add(target_->worldtransform_.translate_, offset);
	}

	if (Camera::isShake_) {
		camera_.SetShake(0.2f, { 0.1f,0.3f,0.1f });
	}

	//ビュー行列の更新
	camera_.UpdateMatrix();
}
