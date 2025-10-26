#include "UniverseCamera.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include"DirectXGame/application/base/Camera/Base/CameraManeger.h"


void UniverseCamera::Initialize(Input* input, Entity3DManager* entity3DManager,  GlobalVariables* globalVariables, Vector3 position)
{
	uniqueCamera_ = std::make_unique<Camera>();
	uniqueCamera_->Initialize(entity3DManager->GetCameraCommon());
	uniqueCamera_->farClip_ = 15000.0f;
	
	uniqueCamera_->transform_.translate = { 3200, 107, 3820 };
	uniqueCamera_->transform_.rotate = { -0.141f, 2.620f, 0.0f };

	
	timer_.maxT = 1.25f;
	
	scaleLerp_.Initialize(strScaleZCamera,endScaleZCamera, 0.0f, 2.0f);
}

void UniverseCamera::Update() {
#ifdef _DEBUG
	ImGui::Begin("Debug");
	ImGui::DragFloat3("translate", &uniqueCamera_->transform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotate", &uniqueCamera_->transform_.rotate.x, 0.01f);
	ImGui::End();
#endif // _DEBUG

	if (useCamera) {
		timer_.Update(GetTime());
		scaleLerp_.Update(GetTime());

		uniqueCamera_->transform_.scale.z = scaleLerp_.LerpData();


		if (timer_.IsMaxOverT()) {
			timer_.t = 0.0f;
			scaleLerp_.currentT = 0.0f;
			cameraManeger->SetUseCamera("followCamera", 0.0f);
		}
	}
	else {

	}
	uniqueCamera_->UpdateMatrix();
}
