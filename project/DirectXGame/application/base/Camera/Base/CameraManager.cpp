#include "CameraManager.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/engine/MyGame/MyGame.h"


CameraManager::~CameraManager()
{
	// シーン破棄時にCameraManager所有のD3Dリソースを確実に解放する
	Finalize();
}

void CameraManager::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables)
{
	this->inputSystem = inputSystem;						// インプット
	this->entityManager = entityManager;	// エンティティ3d
	this->globalVariables = globalVariables;	// 保存項目

	// カメラ初期化
	camera = std::make_unique <Engine::Camera>();
	camera->Initialize(entityManager->GetCameraCommon());
	camera->SetRotate({ 0.36f,0,0 });			// 回転指定
	camera->SetTranslate({ 5,32.5f,-59.2f });	// 位置指定
	camera->SetFarClip(Engine::Camera::kDefaultFarClip);	// 共通のFarクリップ距離を設定
	isGameCamera = true;								// ゲームに使用する

	entityManager->GetObject3dCommon()->SetDefaultCamera(camera.get());					// デフォルトカメラ設定
	entityManager->GetEffectManager()->GetParticleManager()->SetCamera(camera.get());	// デフォルトカメラ設定
	entityManager->GetObject3dCommon()->SetDefaultCamera(camera.get());					// デフォルトカメラ設定
	entityManager->Get3DLineCommon()->SetDefaultCamera(camera.get());						// デフォルトカメラ設定
}

void CameraManager::Finalize()
{
	// 登録カメラは非所有なので、先に参照だけ切る
	cameras.clear();

	// 各描画システムが保持しているCameraの非所有ポインタを無効化する
	if (entityManager) {
		if (entityManager->GetObject3dCommon()) {
			entityManager->GetObject3dCommon()->SetDefaultCamera(nullptr);
		}
		if (entityManager->GetEffectManager()) {
			if (entityManager->GetEffectManager()->GetParticleManager()) {
				entityManager->GetEffectManager()->GetParticleManager()->SetCamera(nullptr);
			}
			if (entityManager->GetEffectManager()->GetGpuParticleManager()) {
				entityManager->GetEffectManager()->GetGpuParticleManager()->SetCamera(nullptr);
			}
		}
		if (entityManager->Get3DLineCommon()) {
			entityManager->Get3DLineCommon()->SetDefaultCamera(nullptr);
		}
		if (entityManager->GetObject3dInstanceManager()) {
			entityManager->GetObject3dInstanceManager()->SetCamera(nullptr);
		}
	}

	// CameraManagerが所有する出力カメラを明示解放する
	if (camera) {
		camera->Finalize();
		camera.reset();
	}

	entityManager = nullptr;
	globalVariables = nullptr;
	inputSystem = nullptr;
	isInterpolating = false;
	currentTime = 0.0f;
}

void CameraManager::Update()
{
	// ImGui更新
	UpdateImGui();

	// ゲームカメラOn
	if (isGameCamera) {

		// 各カメラ更新
		for (auto& cam : cameras) {
			cam.second->Update();
		}

		// 補間中
		if (isInterpolating) {
			// カメラ補間時間はゲーム時間倍率を反映して進める
			currentTime += Engine::MyGame::GameTime();

			float t = std::clamp(currentTime / interpolationTime, 0.0f, 1.0f);

			// 位置と回転を補間
			camera->SetTranslate(Lerp(startTransform.translate, targetTransform.translate, t));
			camera->SetRotate(QuaternionToEuler(Slerp(startTransform.rotate, targetTransform.rotate, t)));
			camera->UpdateMatrix();

			if (t >= 1.0f) {
				isInterpolating = false;
			}
		}
		else {
			// 使っているカメラを更新
			for (auto& cam : cameras) {
				if (cam.second->GetUseCamera()) {
					camera->SetTransform(cam.second->GetUniqueCamera()->GetTransform());
					camera->UpdateMatrix();
				}
			}
		}
	}


	entityManager->GetEffectManager()->GetParticleManager()->SetCamera(camera.get());// デフォルトカメラ設定
	entityManager->GetObject3dCommon()->SetDefaultCamera(camera.get());				// デフォルトカメラ設定
	entityManager->Get3DLineCommon()->SetDefaultCamera(camera.get());					// デフォルトカメラ設定
	entityManager->GetObject3dInstanceManager()->SetCamera(camera.get());				// デフォルトカメラ設定
	// カメラ更新
	camera->UpdateMatrix();
}


void CameraManager::AddCamera(const CameraInfo& cameraInfo, const std::string& name) {
	// CameraManagerは所有権を受け取らず、登録元が所有するカメラを参照する。
	if (!cameraInfo.camera) {
		return;
	}
	// カメラ管理クラスを渡す
	cameraInfo.camera->SetCameraManager(this);
	cameraInfo.camera->SetUseCamera(cameraInfo.useCamera);	// 使っているか
	cameras.insert(std::make_pair(name, cameraInfo.camera));	// カメラ追加
}

void CameraManager::SetUseCamera(const std::string& name, float time) {

	auto it = cameras.find(name);
	if (it != cameras.end()) {

		// 全てのカメラを使用していないことに
		for (auto& cam : cameras) {
			cam.second->SetUseCamera(false);
		}
		// カメラ使用
		it->second->SetUseCamera(true);


		if (time <= 0.0f) {
			// 即時切り替え
			isInterpolating = false;
			camera->SetTransform(it->second->GetUniqueCamera()->GetTransform());
			camera->UpdateMatrix();
		}
		else {
			// 補間開始
			isInterpolating = true;
			currentTime = 0.0f;
			interpolationTime = time;

			// 初期トランスフォーム計算
			startTransform.translate = camera->GetTranslate();
			startTransform.rotate = MakeQuaternionFromEuler(camera->GetRotate());

			// ターゲットトランスフォーム計算
			targetTransform.translate = it->second->GetUniqueCamera()->GetTransform().translate;
			targetTransform.rotate = MakeQuaternionFromEuler(it->second->GetUniqueCamera()->GetTransform().rotate);

		}
	}
}

void CameraManager::UpdateImGui() {

#ifdef _DEBUG
	ImGui::Begin("engine");
	if (ImGui::CollapsingHeader("CameraManager")) {
		Vector3 translate = camera->GetTranslate();
		ImGui::DragFloat3("Translate", &translate.x, 0.1f);	// 位置
		camera->SetTranslate(translate);
		Vector3 rotate = camera->GetRotate();
		ImGui::DragFloat3("Rotate", &rotate.x, 0.01f);		// 回転
		camera->SetRotate(rotate);
		ImGui::Checkbox("isGameCamera", &isGameCamera);							// カメラを使用するか
		bool isPro = camera->GetIsProjection();									// プロジェクション設定
		ImGui::Checkbox("isProjection", &isPro);
		camera->SetIsProjection(isPro);
	}
	ImGui::End();

	ImGui::Begin("Camera Properties");
	if (ImGui::CollapsingHeader("Camera")) {
		ImGui::Separator();
		ImGui::DragFloat("chengeTime", &chengeTime, 0.01f);
		ImGui::Separator();

		// 各カメラのSRT設定
		for (auto& cameraData : cameras) {
			if (ImGui::TreeNode(cameraData.first.c_str())) {
				Vector3 translate = cameraData.second->GetUniqueCamera()->GetTranslate();
				Vector3 rotate = cameraData.second->GetUniqueCamera()->GetRotate();
				Vector3 scale = cameraData.second->GetUniqueCamera()->GetScale();
				ImGui::DragFloat3("Translate", &translate.x, 0.1f);
				ImGui::DragFloat3("Rotate", &rotate.x, 0.01f);
				ImGui::DragFloat3("Scale", &scale.x, 0.01f);
				cameraData.second->GetUniqueCamera()->SetTransform({scale,rotate,translate});

				// 使う
				if (ImGui::Button("use")) {
					SetUseCamera(cameraData.first.c_str(), chengeTime);
				}

				ImGui::TreePop();
			}
		}
	}
	ImGui::End();

#endif
}

BaseCamera* CameraManager::GetBaseCamera() {
	// 使っているカメラを更新
	for (auto& cam : cameras) {
		if (cam.second->GetUseCamera()) {
			return cam.second;
		}
	}
	return nullptr;
}

void CameraManager::DeleteCamera(const std::string& name)
{
	auto it = cameras.find(name);
	if (it != cameras.end()) {
		// カメラ本体は登録元のunique_ptrが所有するため、非所有参照だけを削除する。
		cameras.erase(it);
	}
}
