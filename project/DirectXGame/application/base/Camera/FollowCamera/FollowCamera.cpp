#include "FollowCamera.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include"DirectXGame/engine/input/Input.h"
#include "DirectXGame/engine/Camera/CameraCommon.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

void FollowCamera::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, 
    Engine::GlobalVariables* globalVariables, Vector3 position)
{
    // インプット
   this->inputSystem = inputSystem;

   // カメラ初期化
   uniqueCamera_ = std::make_unique<Engine::Camera>();
   uniqueCamera_->Initialize(entityManager->GetCameraCommon());
   uniqueCamera_->SetFarClip(provisionalData_.farClip_);

   // カメラ回転設定
   uniqueCamera_->SetRotate(provisionalData_.rotate);

  

    uniqueCamera_->AddEffectBlock("bloom", Engine::PostEffectBlockType::kBloom);

    uniqueCamera_->GetPostEffectBlocks()[0]->GetRenderTextures(0)->GetPostEffectData()->GetBloom()->Data()->intensity = provisionalData_.bloomIndensity;
    uniqueCamera_->GetPostEffectBlocks()[0]->GetRenderTextures(1)->GetPostEffectData()->GetGaussian()->Data()->num = provisionalData_.gaussianNum;
    uniqueCamera_->GetPostEffectBlocks()[0]->GetRenderTextures(1)->GetPostEffectData()->GetGaussian()->Data()->sigma = provisionalData_.gaussianSigma;

    // 操作
    controller = std::make_unique<CameraController>();
    controller->Initialize(uniqueCamera_.get(), inputSystem);


    baseOffset.z = -22.5;
}

void FollowCamera::Update() {
    Transform transform = uniqueCamera_->GetTransform();

    // カメラを使っているなら
    if (useCamera) {
        uniqueCamera_->GetPostEffectManager()->AddEffectBlocks(uniqueCamera_->GetPostEffectBlocks());
    }
    // ターゲットがあるなら
    if (target_) {
        controller->SetTraget(target_);
        // 回転速度
        const float kRotateSpeed = 0.03f;
        const float kRotateSpeedX = 0.02f;
        Vector3 targetPos = target_->GetWorldPosition();

        // ロックオンするか
        if (lockOnObject) {
            // ロックオン時の処理
           
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
            transform.rotate.y = targetYaw;
            transform.rotate.x = std::clamp(targetPitch,provisionalData_.rotateMinX, provisionalData_.rotateMaxX);
        }
        else {
            // 通常の自由操作
            if (inputSystem->GetPlayerInputData().isControllerConnected) {
                transform.rotate.y += inputSystem->GetPlayerInputData().lookStick.x * kRotateSpeed;
                transform.rotate.x -= inputSystem->GetPlayerInputData().lookStick.y * kRotateSpeedX;

                transform.rotate.x = std::clamp(transform.rotate.x,provisionalData_.rotateMinX, provisionalData_.rotateMaxX);
            }
        }

        // 回転適用 → カメラ位置を計算
        Matrix4x4 rotY = MakeRotateYMatrix(transform.rotate.y);
        Matrix4x4 rotX = MakeRotateXMatrix(transform.rotate.x);
        Matrix4x4 rotateMatrix = rotX * rotY;
        Vector3 offset = TransformNormal(baseOffset, rotateMatrix);

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
        transform.translate = desiredCameraPos;
    }

    
    // カメラ更新
    uniqueCamera_->SetTransform(transform);
    uniqueCamera_->UpdateMatrix();
}
