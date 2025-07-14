#include "CameraManeger.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"


void CameraManeger::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables)
{
	input_ = input;
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;


	camera = std::make_unique <Camera>();
	camera->Initialize(entity3DManager_->GetCameraCommon());
	camera->transform_.rotate = { 0.36f,0,0 };
	camera->transform_.translate = { 5,32.5f,-59.2f };

	isGameCamera = true;

	entity3DManager_->GetObject3dCommon()->SetDefaltCamera(camera.get());
	entity3DManager_->GetEffectManager()->GetParticleManager()->SetCamera(camera.get());
	entity3DManager_->GetObject3dCommon()->SetDefaltCamera(camera.get());
	entity3DManager_->Get3DLineCommon()->SetDefaltCamera(camera.get());
}

void CameraManeger::Update()
{
	UpadateImGui();

	if (isGameCamera) {

		// 各カメラ更新
		for (auto& cam : cameras) {
			cam.second.camera->Update();
		}

		if (isInterpolating) {
			currentTime += 1.0f / 60.0f; // 仮に60FPS固定

			float t = std::clamp(currentTime / interpolationTime, 0.0f, 1.0f);

			// 位置と回転を補間
			camera->transform_.translate = Lerp(startTransform.translate, targetTransform.translate, t);
			camera->transform_.rotate = Lerp(startTransform.rotate, targetTransform.rotate, t);
			camera->UpdateMatrix();

			if (t >= 1.0f) {
				isInterpolating = false;
			}
		}
		else {
			for (auto& cam : cameras) {
				if (cam.second.useCamera) {
					camera->transform_ = cam.second.camera->GetUniqueCamera()->GetTransform();
					camera->UpdateMatrix();
				}
			}
		}


	}
	else {
		entity3DManager_->GetEffectManager()->GetParticleManager()->SetCamera(camera.get());
		entity3DManager_->GetObject3dCommon()->SetDefaltCamera(camera.get());
		entity3DManager_->Get3DLineCommon()->SetDefaltCamera(camera.get());
		camera->UpdateMatrix();
	}
}

void CameraManeger::AddCamera(CameraInfo camera, std::string name)
{
	cameras.insert(std::make_pair(name, camera));
}

void CameraManeger::SetUseCamera(std::string name, float time)
{
	auto it = cameras.find(name);
	if (it != cameras.end()) {
		for (auto& cam : cameras) {
			cam.second.useCamera = false;
		}
		it->second.useCamera = true;

		if (time <= 0.0f) {
			// 即時切り替え
			isInterpolating = false;
			camera->transform_ = it->second.camera->GetUniqueCamera()->GetTransform();
			camera->UpdateMatrix();
		}
		else {
			// 補間開始
			isInterpolating = true;
			currentTime = 0.0f;
			interpolationTime = time;

			startTransform = camera->transform_;
			targetTransform = it->second.camera->GetUniqueCamera()->GetTransform();
		}
	}
}

void CameraManeger::UpadateImGui()
{

#ifdef _DEBUG
	ImGui::Begin("engine");
	if (ImGui::CollapsingHeader("Camera")) {
		ImGui::DragFloat3("Translate", &camera->transform_.translate.x, 0.1f);
		ImGui::DragFloat3("Rotate", &camera->transform_.rotate.x, 0.01f);
		ImGui::Checkbox("isGameCamera", &isGameCamera);
		if (ImGui::Button("cameraPos")) {
			camera->transform_.translate = { 0,20,-175 };
			camera->transform_.rotate = { 0,0,0 };
		}
		if (ImGui::Button("cameraPos2")) {
			camera->transform_.translate = { -30,10,-140 };
			camera->transform_.rotate = { 0,0,0 };
		}
		if (ImGui::Button("cameraPos3")) {
			camera->transform_.translate = { 0,500,0 };
			camera->transform_.rotate = { DegreesToRadians(90),0,0 };
		}
		if (ImGui::Button("cameraPos4")) {
			camera->transform_.translate = { 0,60,-220 };
			camera->transform_.rotate = { DegreesToRadians(10),0,0 };
		}


	}

	if (ImGui::TreeNode("Test")) {
		ImGui::Text("Camera1");
		ImGui::SliderFloat("値", &camera->transform_.rotate.x, 0.0f, 1.0f);
		//ImGui::TreePop();
		ImGui::Text("Camera2");
		ImGui::SliderFloat("値", &camera->transform_.rotate.x, 0.0f, 1.0f);
		ImGui::TreePop();
	}


	ImGui::End();

#endif
}
