#include "FixedCamera.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void FixedCamera::Initialize(Input* input, Entity3DManager* entity3DManager,  GlobalVariables* globalVariables, Vector3 position)
{
	entity3DManager_ = entity3DManager;	// エンティティ3d

	// カメラ初期化
	uniqueCamera_ = std::make_unique<Camera>();
	uniqueCamera_->Initialize(entity3DManager->GetCameraCommon());
	uniqueCamera_->farClip_ = 15000.0f;

	// カメラ位置と回転設定
	uniqueCamera_->transform_.translate = { 0, 107, 0 };
	uniqueCamera_->transform_.rotate = { 0.341f, 0.0f, 0.0f };


	// ポストエフェクト追加
	uniqueCamera_->AddEffectBlock("out", PostEffectBlockType::kOitline, true);
	uniqueCamera_->AddEffectBlock("copy", PostEffectBlockType::kCopy, true);

}

void FixedCamera::Update()
{
	// カメラを使っているなら
	if (useCamera) {
		uniqueCamera_->GetPostEffectManager()->AddEffectBlocks(uniqueCamera_->GetPostEffectBlocks());
	}
#ifdef _DEBUG
	ImGui::Begin("FixedCamera");
	ImGui::DragFloat3("translate", &uniqueCamera_->transform_.translate.x, 0.1f);	// 位置
	ImGui::DragFloat3("rotate", &uniqueCamera_->transform_.rotate.x, 0.01f);		// 回転
	ImGui::End();

	// デバッグラインを表示
	entity3DManager_->Get3DLineCommon()->GetLineMeshData().AddCameraLine(*uniqueCamera_.get());
#endif // _DEBUG

	// カメラ更新
	uniqueCamera_->UpdateMatrix();
}
