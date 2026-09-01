#include "BaseCamera.h"

#include "DirectXGame/engine/MyGame/MyGame.h"
// 汎用カメラを初期化する。
void BaseCamera::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
	Engine::GlobalVariables* globalVariables, Vector3 position)
{
	// 初期化に使用する外部システムを保持する。
	this->inputSystem = inputSystem;
	this->entityManager = entityManager;
	this->globalVariables = globalVariables;

	// レベルデータなどから直接利用できる標準カメラを生成する。
	Transform transform{};
	transform.translate = position;
	CreateFixedCamera(entityManager, transform, Engine::Camera::kDefaultFarClip);
}

// 特別な挙動を持たない汎用カメラを更新する。
void BaseCamera::Update()
{
	// カメラの行列だけを更新し、派生クラス固有の制御は実行しない。
	if (uniqueCamera_) {
		uniqueCamera_->UpdateMatrix();
	}
}


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

