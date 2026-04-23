#include "FixedCamera.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

void FixedCamera::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables, Vector3 position)
{
	this->entityManager = entityManager;	// エンティティ3d

	// カメラ初期化
	uniqueCamera_ = std::make_unique<Engine::Camera>();
	uniqueCamera_->Initialize(entityManager->GetCameraCommon());
	uniqueCamera_->SetFarClip(provisionalData_.farClip_);

	// カメラ位置と回転設定
	uniqueCamera_->SetTranslate(provisionalData_.translate);
	uniqueCamera_->SetRotate(provisionalData_.rotate);
	

	// ポストエフェクト追加
	uniqueCamera_->AddEffectBlock("out", Engine::PostEffectBlockType::kOitline, true);
	uniqueCamera_->AddEffectBlock("copy", Engine::PostEffectBlockType::kCopy, true);

}

void FixedCamera::Update()
{
	// カメラを使っているなら
	if (useCamera) {
		uniqueCamera_->GetPostEffectManager()->AddEffectBlocks(uniqueCamera_->GetPostEffectBlocks());
	}
#ifdef _DEBUG
	// デバッグラインを表示
	entityManager->Get3DLineCommon()->GetLineMeshData().AddCameraLine(*uniqueCamera_.get());
#endif // _DEBUG

	// カメラ更新
	uniqueCamera_->UpdateMatrix();
}
