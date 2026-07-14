#include "EffectCamera.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

void EffectCamera::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, 
	Engine::GlobalVariables* globalVariables, Vector3 position){
	CreateFixedCamera(entityManager, { {1,1,1},provisionalData_.rotate,provisionalData_.translate },
		provisionalData_.farClip_);
}

void EffectCamera::Update() {
	// カメラを使っているなら
	if (useCamera) {
		uniqueCamera_->GetPostEffectManager()->AddPipeline(uniqueCamera_->GetPostEffectPipeline());
	}
#ifdef _DEBUG
	// デバッグラインを表示
	entityManager->Get3DLineCommon()->GetLineMeshData().AddCameraLine(*uniqueCamera_.get());
#endif // _DEBUG

	// カメラ更新
	uniqueCamera_->UpdateMatrix();
}
