#pragma once
#include "BaseStage.h"

// 前方宣言
namespace Engine {
	class DirectXCommon;
	class EntityManager;
	class GlobalVariables;
}
class EffectSystem;

/// <summary>
/// チュートリアル用ステージ
/// </summary>
class TutorialStage : public BaseStage {
public:
	// 初期化
	void Initialize(Engine::EntityManager* entityManager, CameraManager* cameraManager) override;

	// 更新
	void Update(float dt) override;
private:
	/// <summary>
	/// エミッター初期化
	/// </summary>
	void InitEmit() {};

	/// <summary>
	/// エミッター更新
	/// </summary>
	void EmitUpdate() {};

public:
	/// <summary>
	/// エフェクト
	/// </summary>
	/// <param name="effect"></param>
	void SetEffect(EffectSystem* effect) {
		this->effect = effect;
	}
private:

};
