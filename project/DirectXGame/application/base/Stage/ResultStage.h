#pragma once
#include "BaseStage.h"

/// <summary>
/// リザルト用ステージクラス
/// </summary>
class ResultStage : public BaseStage {
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

private:
	// プレイヤー車
	std::unique_ptr<PlayerCar> playerCar_;
	// 換金所
	Engine::Object3d* moneyExchangePlace;
private:
	// 位置
	Vector3 playerCarPos_ = { -1.25f,0.05f,-4.0f };
};