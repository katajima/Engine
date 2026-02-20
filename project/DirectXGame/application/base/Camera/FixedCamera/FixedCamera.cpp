#include "FixedCamera.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void FixedCamera::Initialize(InputSystem* inputSystem, Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables, Vector3 position)
{
	this->entity3DManager = entity3DManager;	// エンティティ3d

	// カメラ初期化
	uniqueCamera_ = std::make_unique<Engine::Camera>();
	uniqueCamera_->Initialize(entity3DManager->GetCameraCommon());
	uniqueCamera_->farClip_ = provisionalData_.farClip_;

	// カメラ位置と回転設定
	uniqueCamera_->transform_.translate = provisionalData_.translate;
	uniqueCamera_->transform_.rotate = provisionalData_.rotate;


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
	entity3DManager->Get3DLineCommon()->GetLineMeshData().AddCameraLine(*uniqueCamera_.get());
#endif // _DEBUG

	// カメラ更新
	uniqueCamera_->UpdateMatrix();
}
