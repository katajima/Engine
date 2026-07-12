#pragma once
#include "BaseStage.h"

class CustomStage : public BaseStage {
public:
	// 初期化
	void Initialize(Engine::EntityManager* entityManager, Engine::AudioManager* audioManager, CameraManager* cameraManager) override;
	// 更新
	void Update(float dt) override;
private:

};
