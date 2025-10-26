#include "FixedCamera.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void FixedCamera::Initialize(Input* input, Entity3DManager* entity3DManager,  GlobalVariables* globalVariables, Vector3 position)
{
	entity3DManager_ = entity3DManager;

	uniqueCamera_ = std::make_unique<Camera>();
	uniqueCamera_->Initialize(entity3DManager->GetCameraCommon());
	uniqueCamera_->farClip_ = 15000.0f;

	uniqueCamera_->transform_.translate = { 0, 107, 0 };
	uniqueCamera_->transform_.rotate = { 0.341f, 0.0f, 0.0f };


	//uniqueCamera_->AddEffectBlock("grayScale", PostEffectBlockType::kGrayScale, false);
	uniqueCamera_->AddEffectBlock("out", PostEffectBlockType::kOitline, true);
	uniqueCamera_->AddEffectBlock("copy", PostEffectBlockType::kCopy, true);

}

void FixedCamera::Update()
{
	if (useCamera) {
		uniqueCamera_->GetPostEffectManager()->AddEffectBlocks(uniqueCamera_->GetPostEffectBlocks());
	}
#ifdef _DEBUG
	ImGui::Begin("FixedCamera");
	ImGui::DragFloat3("translate", &uniqueCamera_->transform_.translate.x, 0.1f);
	ImGui::DragFloat3("rotate", &uniqueCamera_->transform_.rotate.x, 0.01f);
	ImGui::End();

	entity3DManager_->Get3DLineCommon()->GetLineMeshData().AddCameraLine(*uniqueCamera_.get());
#endif // _DEBUG


	uniqueCamera_->UpdateMatrix();
}
