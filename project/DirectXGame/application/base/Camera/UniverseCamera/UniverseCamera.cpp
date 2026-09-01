#include "UniverseCamera.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include"DirectXGame/engine/Camera/CameraManager.h"


void UniverseCamera::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, 
	Engine::GlobalVariables* globalVariables, Vector3 position){
	// カメラ初期化
	uniqueCamera_ = std::make_unique<Engine::Camera>();
	uniqueCamera_->Initialize(entityManager->GetCameraCommon());
	uniqueCamera_->SetFarClip(provisionalData_.farClip_);

	// カメラ位置と回転設定
	uniqueCamera_->SetTranslate(provisionalData_.translate);
	uniqueCamera_->SetRotate(provisionalData_.rotate);

	// 起動時間
	timer_.maxT = provisionalData_.maxT;
	
	// 補間設定
	scaleLerp_.Initialize(strScaleZCamera,endScaleZCamera, 0.0f, provisionalData_.scaleZCamera);
}

void UniverseCamera::Update() {
	Transform transform = uniqueCamera_->GetTransform();

	// カメラを使っているなら
	if (useCamera) {
		timer_.Update(GetTime());		// タイマー更新
		scaleLerp_.Update(GetTime());	// スケール補間更新

		// カメラスケールZを設定
		transform.scale.z = scaleLerp_.LerpData();


		// 時間オーバーしたら
		if (timer_.IsMaxOverT()) {
			timer_.t = 0.0f;			// 初期化
			scaleLerp_.currentT = 0.0f;	// 初期化
			cameraManager->SetUseCamera("followCamera", 0.0f);	// フォローカメラに移行
		}
	}
	
	// カメラ更新
	uniqueCamera_->SetTransform(transform);
	uniqueCamera_->UpdateMatrix();
}
