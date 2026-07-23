#pragma once
#include "BaseStage.h"


/// <summary>
/// ステージクラス
/// </summary>
class MainStage : public BaseStage {
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
	//上限範囲
	Engine::Object3d* stageLimit_ = nullptr;
private:
	// プレイヤー車
	std::unique_ptr<PlayerCar> playerCar_;
	// 位置
	Vector3 playerCarPos_ = { 120.0f,-3.00f,0.0f };
};
