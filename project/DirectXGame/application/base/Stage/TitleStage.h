#pragma once
#include "BaseStage.h"

/// <summary>
/// タイトルシーンステージ
/// </summary>
class TitleStage : public BaseStage {
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
	//
	void InitializeStone();
private:
	// タイル
	Engine::Object3d* tail;
	// 石
	Engine::Object3d* stone1_;
	Engine::Object3d* stone2_;
	Engine::Object3d* stone3_;
	Engine::Object3d* stone4_;
	Engine::Object3d* stone5_;
	// プレイヤー車
	std::unique_ptr<PlayerCar> playerCar_;
	// 位置
	Vector3 playerCarPos_ = { -1.25f,0.05f,12.0f };
private:
	struct ProvisionalData {
		Vector3 oceanTranslate = { 0, -30, 0 };
		Vector3 oceanRotate = { Math::DegreesToRadians(90) ,0,0 };

		Vector3 skyBoxScale = { 1000,1000,1000 };


		Vector2 oceanRange{ 8000.0f,8000.0f };
		float oceanAmplitude = 16.5f;
		Vector2 waveDirection = { 0.5f,0.5f };
		float waveSpeed = 5.0f;
		Color color = { 0.0f, 0.0f, 0.8f, 0.75f };

		int missileNumX = 5;
		int missileNumY = 2;

		Vector3 missileTranslate = { 3500,106,3000 };
		Vector2 missileInterval = { 120.0f,100.0f };
		Vector3 missileRotate = { 0 ,Math::DegreesToRadians(-90),0 };
		float  missileSize = 10.0f;
	};
	ProvisionalData provisionalData_;
};