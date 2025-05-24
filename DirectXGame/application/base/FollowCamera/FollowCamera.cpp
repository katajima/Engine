#include "FollowCamera.h"

#include"DirectXGame/engine/input/Input.h"
#include "DirectXGame/engine/Camera/CameraCommon.h"

void FollowCamera::Initialize(CameraCommon* cameraCommon)
{
	input_ = cameraCommon->GetInput();

	camera_.Initialize(cameraCommon);
	camera_.farClip_ = 5000.0f;
	camera_.transform_.rotate.x = DegreesToRadians(90);
	camera_.transform_.rotate.x = DegreesToRadians(20);

}

void FollowCamera::Update()
{
    if (target_) {

        const float kRotateSpeed = 0.03f;

        if (input_->IsControllerConnected()) {
            camera_.transform_.rotate.y += input_->GetGamePadRightStick().x * kRotateSpeed;
            camera_.transform_.rotate.x += input_->GetGamePadRightStick().y * kRotateSpeed;

            camera_.transform_.rotate.x = std::clamp(camera_.transform_.rotate.x, DegreesToRadians(-15.0f), DegreesToRadians(60.0f));
        }
        else {
            if (input_->IsPushKey(DIK_LEFT)) {
                camera_.transform_.rotate.y -= 0.01f;
            }
            if (input_->IsPushKey(DIK_RIGHT)) {
                camera_.transform_.rotate.y += 0.01f;
            }

            camera_.transform_.rotate.x = std::clamp(camera_.transform_.rotate.x, DegreesToRadians(0.0f), DegreesToRadians(60.0f));
        }

        // ベースのオフセット（固定距離）
        Vector3 baseOffset = { 0.0f, 5.0f, -50.0f };

        // 回転適用
        Matrix4x4 rotY = MakeRotateYMatrix(camera_.transform_.rotate.y);
        Matrix4x4 rotX = MakeRotateXMatrix(camera_.transform_.rotate.x);
        Matrix4x4 rotateMatrix = rotX * rotY;
        Vector3 offset = TransformNormal(baseOffset, rotateMatrix);

        Vector3 targetPos = target_->worldtransform_.translate_;
        Vector3 desiredCameraPos = Add(targetPos, offset);

        // 地面以下にカメラが沈んでいる場合のみ、Zを近づけて補正
        if (desiredCameraPos.y < 0.0f) {
            float depth = -desiredCameraPos.y; // どれだけ沈んでいるか
            float maxZOffset = 30.0f; // 最大どれだけZを近づけるか（調整可）
            float zAdjust = std::clamp(depth * 2.0f, 0.0f, maxZOffset); // 線形補間

            // カメラ方向を正規化
            Vector3 direction = Normalize(Subtract(targetPos, desiredCameraPos));
            Vector3 zOffset = Multiply(direction, zAdjust);

            // 補正を加える
            desiredCameraPos = Add(desiredCameraPos, zOffset);

            // 地面に出るようにYを補正
            desiredCameraPos.y = 0.0f;
        }

        camera_.transform_.translate = desiredCameraPos;
    }

    if (Camera::isShake_) {
        camera_.SetShake(0.2f, { 0.1f, 0.3f, 0.1f });
    }

    camera_.UpdateMatrix();
}


