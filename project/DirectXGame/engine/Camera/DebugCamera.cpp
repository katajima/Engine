#include "DebugCamera.h"

#include "DirectXGame/engine/Camera/CameraCommon.h"

void Engine::DebugCamera::Initialize(CameraCommon* cameraCommon)
{
    // カメラ
	camera_.Initialize(cameraCommon);
	camera_.SetFarClip(5000.0f);
	
    // インプット
    input = cameraCommon->GetInput();
}

void Engine::DebugCamera::Update() {
    Transform transform = camera_.GetTransform();
    

    // マウスの移動量を取得
    Vector2 mouseDelta = input->GetMousePosition();
    static Vector2 prevMousePos = mouseDelta;
    Vector2 delta = { mouseDelta.x - prevMousePos.x, mouseDelta.y - prevMousePos.y };
    prevMousePos = mouseDelta;

    // 右クリックでカメラ回転
    if (input->IsMousePressed(1)) {
        float rotationSpeed = 0.005f;
        transform.rotate.y -= delta.x * rotationSpeed;
        transform.rotate.x -= delta.y * rotationSpeed;

        // ピッチ（上下）の制限
        transform.rotate.x = std::clamp(transform.rotate.x, -1.5f, 1.5f);
    }

    // 中クリックまたは Shift + 右クリックで平行移動
    if (input->IsMousePressed(2) || (input->IsMousePressed(1) && input->IsPushKey(VK_SHIFT))) {
        float moveSpeed = 0.01f;
        Vector3 forward = TransformNormal({ 0, 0, 1 }, camera_.GetViewMatrix());
        Vector3 right = TransformNormal({ 1, 0, 0 }, camera_.GetViewMatrix());

        transform.translate -= right * delta.x * moveSpeed;
        transform.translate += forward * delta.y * moveSpeed;
    }

    // マウスホイールでズーム
    float zoomSpeed = 0.1f;
    if (input->IsMousePressed(VK_MBUTTON)) {
        transform.translate.z += zoomSpeed;
     }

    camera_.SetTransform(transform);
	camera_.UpdateMatrix();
}
