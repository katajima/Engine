#pragma once
#include "BaseStage.h"

/// <summary>
/// CustomStageを管理・実装するクラス。
/// </summary>
class CustomStage : public BaseStage {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Engine::EntityManager* entityManager, Engine::AudioManager* audioManager, CameraManager* cameraManager) override;
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt) override;
private:

};
