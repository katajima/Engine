#pragma once
#include "BaseStage.h"


/// <summary>
/// ステージクラス
/// </summary>
class MainStage : public BaseStage {
public:
	// 初期化
	void Initialize(Engine::EntityManager* entityManager, Engine::AudioManager* audioManager, CameraManager* cameraManager) override;

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
	std::vector<Engine::Object3d*> missiles_;
	//上限範囲
	Engine::Object3d* stageLimit_ = nullptr;


private:
	// プレイヤー車
	std::unique_ptr<PlayerCar> playerCar_;
	// 位置
	Vector3 playerCarPos_ = { 120.0f,-3.00f,0.0f };
private: // 一旦
	struct ProvisionalData {
		int missileNumX = 5;
		int missileNumY = 2;

		Vector3 missileTranslate = { 3500,106,3000 };
		Vector2 missileInterval = { 120.0f,100.0f };
		Vector3 missileRotate = { 0 ,Math::DegreesToRadians(-90),0 };
		float  missileSize = 10.0f;
	};
	ProvisionalData provisionalData_;
};
