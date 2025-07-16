#pragma once
#include "BaseCamera.h"


struct CameraInfo {
	BaseCamera* camera;
	bool useCamera = false;
};

class CameraManeger
{
public:

	// 初期化
	void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables);

	// 更新
	void Update();

	// カメラセット
	void AddCamera(CameraInfo camera,std::string name);

	// 使っているカメラ設定(string カメラの名前、float 切り替わり時間(補間時間))
	void SetUseCamera(std::string name,float time);

	void UpadateImGui();

private:
	// 全てのカメラ
	std::map<std::string, BaseCamera*> cameras;
	// カメラ
	std::unique_ptr < Camera> camera;
	// ゲームでの画面か
	bool isGameCamera = true;

	// カメラ補間状態
	bool isInterpolating = false;
	float interpolationTime = 0.0f;
	float currentTime = 0.0f;

	//Transform;

	QuaternionTransform startTransform;
	QuaternionTransform targetTransform;



private:
	Entity3DManager* entity3DManager_ = nullptr;	// 3Dエンティティマネージャー
	Entity2DManager* entity2DManager_ = nullptr;	// 2Dエンティティマネージャー
	GlobalVariables* globalVariables_ = nullptr;	// グローバル変数
	Camera* camera_ = nullptr;						// カメラ
	Input* input_ = nullptr;						// 入力
};
