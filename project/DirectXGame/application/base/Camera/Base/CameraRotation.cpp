#include "CameraRotation.h"

void CameraRotation::Initialize() {
    // 初期角度をリセット（必要なら外部から設定してもOK）
    data_.yaw = 0.0f;
    data_.pitch = 0.0f;
};


Vector3 CameraRotation::Update(const InputSystem* input, float dt) {
    if (!input) {
        return Vector3(0.0f, 0.0f, 1.0f);
    }
    // =========================
    // 入力取得（例：マウスや右スティック）
    // =========================
    float inputX = input->GetPlayerInputData().lookStick.y;
    float inputY = input->GetPlayerInputData().lookStick.x; 


    // =========================
   // 回転更新
   // =========================
    data_.yaw += inputX * data_.sensitivity;
    data_.pitch += inputY * data_.sensitivity;

    // =========================
    // ピッチ制限
    // =========================
    data_.pitch = std::clamp(data_.pitch, data_.minPitch, data_.maxPitch);

    // =========================
   // 方向ベクトル計算（Y-Up / Z-Forward）
   // =========================
    Vector3 forward{};

    forward.x = cosf(data_.pitch) * sinf(data_.yaw);
    forward.y = sinf(data_.pitch);
    forward.z = cosf(data_.pitch) * cosf(data_.yaw);

    forward = Normalize(forward);

    return forward;
}
