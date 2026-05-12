#include "CameraRotation.h"
#include <DirectXGame/application/base/Input/InputSystem.h>

void CameraRotation::Initialize() {
    // 初期角度をリセット（必要なら外部から設定してもOK）
    data_.yaw = 0.0f;
    data_.pitch = Math::DegreesToRadians(20);
    data_.maxPitch = Math::DegreesToRadians(20);
    data_.minPitch = Math::DegreesToRadians(-10);

};


void CameraRotation::Update(Transform& transform,const InputSystem* input, float dt) {
    if (!input) {
        return;
    }
    // =========================
    // 入力取得（例：マウスや右スティック）
    // =========================
    float inputX = input->GetPlayerInputData().lookStick.x;
    float inputY = input->GetPlayerInputData().lookStick.y; 


    // =========================
   // 回転更新
   // =========================
    data_.yaw += inputX * data_.sensitivity;
    data_.pitch -= inputY * data_.sensitivity;

    // =========================
    // ピッチ制限
    // =========================
    data_.pitch = std::clamp(data_.pitch, data_.minPitch, data_.maxPitch);

    transform.rotate.x = data_.pitch;
    transform.rotate.y = data_.yaw;
    transform.rotate.z = {};

}
