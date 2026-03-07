#include "UniverseCamera.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include"DirectXGame/application/base/Camera/Base/CameraManeger.h"


void UniverseCamera::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, 
	Engine::GlobalVariables* globalVariables, Vector3 position)
{
	// カメラ初期化
	uniqueCamera_ = std::make_unique<Engine::Camera>();
	uniqueCamera_->Initialize(entityManager->GetCameraCommon());
	uniqueCamera_->farClip_ = provisionalData_.farClip_;
	
	// カメラ位置回転設定
	uniqueCamera_->transform_.translate = provisionalData_.translate;
	uniqueCamera_->transform_.rotate = provisionalData_.rotate;

	// 起動時間
	timer_.maxT = provisionalData_.maxT;
	
	// 補間設定
	scaleLerp_.Initialize(strScaleZCamera,endScaleZCamera, 0.0f, provisionalData_.scaleZCamera);
}

void UniverseCamera::Update() {
#ifdef _DEBUG
	ImGui::Begin("Debug");
	ImGui::DragFloat3("translate", &uniqueCamera_->transform_.translate.x, 0.1f);	// 位置
	ImGui::DragFloat3("rotate", &uniqueCamera_->transform_.rotate.x, 0.01f);		// 回転
	ImGui::End();
#endif // _DEBUG

	// カメラを使っているなら
	if (useCamera) {
		timer_.Update(GetTime());		// タイマー更新
		scaleLerp_.Update(GetTime());	// スケール補間更新

		// カメラスケールZを設定
		uniqueCamera_->transform_.scale.z = scaleLerp_.LerpData();


		// 時間オーバーしたら
		if (timer_.IsMaxOverT()) {
			timer_.t = 0.0f;			// 初期化
			scaleLerp_.currentT = 0.0f;	// 初期化
			cameraManeger->SetUseCamera("followCamera", 0.0f);	// フォローカメラに移行
		}
	}
	else {

	}
	// カメラ更新
	uniqueCamera_->UpdateMatrix();
}
