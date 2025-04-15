#include "DebugCamera.h"

#include "DirectXGame/engine/Camera/CameraCommon.h"

void DebugCamera::Initialize(CameraCommon* cameraCommon)
{
	camera_.Initialize(cameraCommon);
	camera_.farClip_ = 5000.0f;
	//camera_.transform_.rotate.x = DegreesToRadians(90);
	//camera_.transform_.rotate.x = DegreesToRadians(20);

    input_ = cameraCommon->GetInput();
}

void DebugCamera::Update()
{
    

    // マウスの移動量を取得
    Vector2 mouseDelta = input_->GetMousePosition();
    static Vector2 prevMousePos = mouseDelta;
    Vector2 delta = { mouseDelta.x - prevMousePos.x, mouseDelta.y - prevMousePos.y };
    prevMousePos = mouseDelta;

    // 右クリックでカメラ回転
    if (input_->IsMousePressed(1)) {
        float rotationSpeed = 0.005f;
        camera_.transform_.rotate.y -= delta.x * rotationSpeed;
        camera_.transform_.rotate.x -= delta.y * rotationSpeed;

        // ピッチ（上下）の制限
        camera_.transform_.rotate.x = std::clamp(camera_.transform_.rotate.x, -1.5f, 1.5f);
    }

    // 中クリックまたは Shift + 右クリックで平行移動
    if (input_->IsMousePressed(2) || (input_->IsMousePressed(1) && input_->IsPushKey(VK_SHIFT))) {
        float moveSpeed = 0.01f;
        Vector3 forward = TransformNormal({ 0, 0, 1 }, camera_.GetViewMatrix());
        Vector3 right = TransformNormal({ 1, 0, 0 }, camera_.GetViewMatrix());

        camera_.transform_.translate -= right * delta.x * moveSpeed;
        camera_.transform_.translate += forward * delta.y * moveSpeed;
    }

    // マウスホイールでズーム
    float zoomSpeed = 0.1f;
    if (input_->IsMousePressed(VK_MBUTTON)) {
        camera_.transform_.translate.z += zoomSpeed;
     }

    
	camera_.UpdateMatrix();
}
