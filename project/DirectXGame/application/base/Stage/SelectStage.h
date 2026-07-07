#pragma once
#include "BaseStage.h"

/// <summary>
/// セレクト用ステージ
/// </summary>
class SelectStage : public BaseStage {
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
		playerCar_->SetEffect(effect);
	}
	/// <summary>
	/// 車
	/// </summary>
	/// <returns></returns>
	PlayerCar* GetPlayerCar() { return playerCar_.get(); }

	void SetRatio(float ratio) { ratio_ = ratio; }
private:
	// プレイヤー車
	std::unique_ptr<PlayerCar> playerCar_;
	// 石
	std::vector<std::unique_ptr<ObjectComponent>> stoneComponents_;
private:
	// 位置
	Vector3 playerCarPos_ = { -1.25f,0.05f,12.0f };
	//
	float ratio_ = 1.0f;
};
