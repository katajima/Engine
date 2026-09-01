#include "TutorialStage.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/engine/Camera/CameraManager.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include "DirectXGame/engine/3d/Object/Object3d.h"

// 初期化
void TutorialStage::Initialize(Engine::EntityManager* entityManager, Engine::AudioManager* audioManager, CameraManager* cameraManager) {
	this->entityManager = entityManager;
	this->cameraManager = cameraManager;
}

// 更新
void TutorialStage::Update(float dt) {

}
