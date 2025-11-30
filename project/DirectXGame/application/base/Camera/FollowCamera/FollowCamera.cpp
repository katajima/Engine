#include "FollowCamera.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include"DirectXGame/engine/input/Input.h"
#include "DirectXGame/engine/Camera/CameraCommon.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

void FollowCamera::Initialize(Input* input, Entity3DManager* entity3DManager,  GlobalVariables* globalVariables, Vector3 position)
{
    // インプット
	input_ = input;

    // カメラ初期化
    uniqueCamera_ = std::make_unique<Camera>();
    uniqueCamera_->Initialize(entity3DManager->GetCameraCommon());
    uniqueCamera_->farClip_ = provisionalData_.farClip_;
    uniqueCamera_->transform_.rotate.x = provisionalData_.rotate.x;

   
    uniqueCamera_->AddEffectBlock("bloom", PostEffectBlockType::kBloom);

    uniqueCamera_->GetPostEffectBlocks()[0]->GetRenderTextures(0)->GetPostEffectData()->GetBloom()->Data()->intensity = provisionalData_.bloomIndensity;
    uniqueCamera_->GetPostEffectBlocks()[0]->GetRenderTextures(1)->GetPostEffectData()->GetGaussian()->Data()->num = provisionalData_.gaussianNum;
    uniqueCamera_->GetPostEffectBlocks()[0]->GetRenderTextures(1)->GetPostEffectData()->GetGaussian()->Data()->sigma = provisionalData_.gaussianSigma;
}

void FollowCamera::Update()
{
    // カメラを使っているなら
    if (useCamera) {
        uniqueCamera_->GetPostEffectManager()->AddEffectBlocks(uniqueCamera_->GetPostEffectBlocks());
    }
    // ターゲットがあるなら
    if (target_) {

        // 回転速度
        const float kRotateSpeed = 0.03f;

        // ロックオンするか
        if (lockOnObject) {
            // ロックオン時の処理
            Vector3 targetPos = target_->worldMat_.GetWorldPosition();
            Vector3 lockOnPos = lockOnObject->GetWorldTransform().worldMat_.GetWorldPosition();

            // カメラ方向をロックオンターゲットに向ける
            Vector3 direction = Subtract(lockOnPos, targetPos);
            direction = Normalize(direction);

            // Y軸角度
            float targetYaw = std::atan2(direction.x, direction.z); // Z前方、X右

            // X軸（仰角）：水平→目標への角度
            float distanceXZ = std::sqrt(direction.x * direction.x + direction.z * direction.z);
            float targetPitch = std::atan2(direction.y, distanceXZ);

            // スムーズに補間（必要なら）
            uniqueCamera_->transform_.rotate.y = targetYaw;
            uniqueCamera_->transform_.rotate.x = std::clamp(targetPitch,provisionalData_.rotateMinX, provisionalData_.rotateMaxX);
        }
        else {
            // 通常の自由操作
            if (input_->IsControllerConnected()) {
                uniqueCamera_->transform_.rotate.y += input_->GetGamePadRightStick().x * kRotateSpeed;
                uniqueCamera_->transform_.rotate.x += input_->GetGamePadRightStick().y * kRotateSpeed;

                uniqueCamera_->transform_.rotate.x = std::clamp(uniqueCamera_->transform_.rotate.x,provisionalData_.rotateMinX, provisionalData_.rotateMaxX);
            }
            else {
                // 回転移動
                if (input_->IsPushKey(DIK_LEFT)) {
                    uniqueCamera_->transform_.rotate.y -= kRotateSpeed;
                }
                // 回転移動
                if (input_->IsPushKey(DIK_RIGHT)) {
                    uniqueCamera_->transform_.rotate.y += kRotateSpeed;
                }

                uniqueCamera_->transform_.rotate.x = std::clamp(uniqueCamera_->transform_.rotate.x, Math::DegreesToRadians(0.0f), provisionalData_.rotateMaxX);
            }
        }

        // 回転適用 → カメラ位置を計算
        Matrix4x4 rotY = MakeRotateYMatrix(uniqueCamera_->transform_.rotate.y);
        Matrix4x4 rotX = MakeRotateXMatrix(uniqueCamera_->transform_.rotate.x);
        Matrix4x4 rotateMatrix = rotX * rotY;
        Vector3 offset = TransformNormal(baseOffset, rotateMatrix);

        Vector3 targetPos = target_->worldMat_.GetWorldPosition();
        Vector3 desiredCameraPos = Add(targetPos, offset);

        // 地面への沈み補正
        if (desiredCameraPos.y < 0.0f) {
            float depth = -desiredCameraPos.y;
           
            float zAdjust = std::clamp(depth * provisionalData_.depthScale, 0.0f, provisionalData_.maxZOffset);

            Vector3 direction = Normalize(Subtract(targetPos, desiredCameraPos));
            Vector3 zOffset = Multiply(direction, zAdjust);

            desiredCameraPos = Add(desiredCameraPos, zOffset);
            desiredCameraPos.y = 0.0f;
        }

        // 位置設定
        uniqueCamera_->transform_.translate = desiredCameraPos;
    }

    // 揺れ処理
    if (Camera::isShake_) {
        uniqueCamera_->SetShake(provisionalData_.shackTime, provisionalData_.shackWidth);
    }

    // カメラ更新
    uniqueCamera_->UpdateMatrix();
}
