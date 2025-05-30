#include "UniverseCamera.h"


void UniverseCamera::Initialize(CameraCommon* cameraCommon) 
{
	camera_.Initialize(cameraCommon);
	camera_.farClip_ = 15000.0f;
	
	camera_.transform_.translate = { 3200, 107, 3820 };
	camera_.transform_.rotate = { -0.141f, 2.620f, 0.0f };
}

void UniverseCamera::Update() {
#ifdef _DEBUG
	ImGui::Begin("Debug");
	ImGui::DragFloat3("translate", &camera_.transform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotate", &camera_.transform_.rotate.x, 0.01f);
	ImGui::End();
#endif // _DEBUG

	camera_.UpdateMatrix();
}