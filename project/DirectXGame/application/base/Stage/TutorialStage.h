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
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Engine::EntityManager* entityManager, Engine::AudioManager* audioManager, CameraManager* cameraManager) override;

	/// <summary>
	/// 更新
	/// </summary>
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
