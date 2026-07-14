#include "BaseCamera.h"

#include "DirectXGame/engine/MyGame/MyGame.h"


/// <summary>
/// オブジェクトの時間取得
/// </summary>
/// <returns></returns>
float BaseCamera::GetTime() const { return Engine::MyGame::GameTime() * timeSpeed_; }

void BaseCamera::CreateFixedCamera(Engine::EntityManager* entityManager, const Transform& transform, float farClip) {
	this->entityManager = entityManager;	// エンティティ3d

	// カメラ初期化
	uniqueCamera_ = std::make_unique<Engine::Camera>();
	uniqueCamera_->Initialize(entityManager->GetCameraCommon());
	uniqueCamera_->SetFarClip(farClip);

	// カメラ位置と回転設定
	uniqueCamera_->SetTranslate(transform.translate);
	uniqueCamera_->SetRotate(transform.rotate);


	// ポストエフェクト追加
	uniqueCamera_->AddEffectBlock("copy", Engine::PostEffectBlockType::kCopy, true);
}

