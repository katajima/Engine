#include "FollowCamera.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include"DirectXGame/engine/input/Input.h"
#include "DirectXGame/engine/Camera/CameraCommon.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

void FollowCamera::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera)
{
	input_ = input;

    uniqueCamera_ = std::make_unique<Camera>();
    uniqueCamera_->Initialize(entity3DManager->GetCameraCommon());
    uniqueCamera_->farClip_ = 15000.0f;
    uniqueCamera_->transform_.rotate.x = DegreesToRadians(90);
    uniqueCamera_->transform_.rotate.x = DegreesToRadians(20);

}

void FollowCamera::Update()
{
    if (target_) {

        const float kRotateSpeed = 0.03f;

        if (input_->IsControllerConnected()) {
            uniqueCamera_->transform_.rotate.y += input_->GetGamePadRightStick().x * rotateSpeed;
            uniqueCamera_->transform_.rotate.x += input_->GetGamePadRightStick().y * rotateSpeed;

            uniqueCamera_->transform_.rotate.x = std::clamp(uniqueCamera_->transform_.rotate.x, DegreesToRadians(-15.0f), DegreesToRadians(60.0f));
        }
        else {
            if (input_->IsPushKey(DIK_LEFT)) {
                uniqueCamera_->transform_.rotate.y -= 0.01f;
            }
            if (input_->IsPushKey(DIK_RIGHT)) {
                uniqueCamera_->transform_.rotate.y += 0.01f;
            }

            uniqueCamera_->transform_.rotate.x = std::clamp(uniqueCamera_->transform_.rotate.x, DegreesToRadians(0.0f), DegreesToRadians(60.0f));
        }

        

        // 回転適用
        Matrix4x4 rotY = MakeRotateYMatrix(uniqueCamera_->transform_.rotate.y);
        Matrix4x4 rotX = MakeRotateXMatrix(uniqueCamera_->transform_.rotate.x);
        Matrix4x4 rotateMatrix = rotX * rotY;
        Vector3 offset = TransformNormal(baseOffset, rotateMatrix);

        Vector3 targetPos = target_->GetWorldTransform().worldMat_.GetWorldPosition();
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

        uniqueCamera_->transform_.translate = desiredCameraPos;
    }



    if (Camera::isShake_) {
        uniqueCamera_->SetShake(0.2f, { 0.1f, 0.3f, 0.1f });
    }

    uniqueCamera_->UpdateMatrix();
}


void FollowCamera::DrawEffect() {

}

void FollowCamera::Draw2D() {

}