#pragma once
#include "BaseStage.h"

class CustomStage : public BaseStage {
public:
	// 初期化
	void Initialize(Engine::EntityManager* entityManager, CameraManager* cameraManager) override;
	// 更新
	void Update(float dt) override;
	///< summary>
	/// 描画
	///</summary>
	void Draw() override {};
	/// <summary>
	/// 描画エフェクト
	/// </summary>
	void DrawEffect() override {};
	/// <summary>
	/// 描画2d
	/// </summary>
	void Draw2D() override {};

private:

};
