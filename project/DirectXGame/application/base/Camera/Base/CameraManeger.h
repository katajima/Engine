#pragma once
#include "BaseCamera.h"

// カメラインフォメーション
struct CameraInfo {
	BaseCamera* camera;
	bool useCamera = false;
};

/// <summary>
/// カメラマネージャークラス
/// </summary>
class CameraManager
{
public:

	// 初期化
	void Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables);

	// 更新
	void Update();

	// カメラセット
	void AddCamera(CameraInfo camera, std::string name);

	// 使っているカメラ設定(string カメラの名前、float 切り替わり時間(補間時間))
	void SetUseCamera(std::string name, float time);
	// ImGui更新
	void UpadateImGui();
	// カメラ取得
	Engine::Camera* GetCamera() { return camera.get(); }
	// カメラを管理対象から削除
	void DeleteCamera(std::string name);
	// カメラクリア
	void Clear() {
		cameras.clear();
	};

private:
	// 全てのカメラ
	std::map<std::string, BaseCamera*> cameras;
	// カメラ
	std::unique_ptr < Engine::Camera> camera;
	// ゲームでの画面か
	bool isGameCamera = true;

	// カメラ補間状態
	bool isInterpolating = false;
	float interpolationTime = 0.0f;
	float currentTime = 0.0f;


	float chengeTime = 0.1f;

	QuaternionTransform startTransform;
	QuaternionTransform targetTransform;



private:
	Engine::Entity3DManager* entity3DManager_ = nullptr;	// 3Dエンティティマネージャー
	Engine::GlobalVariables* globalVariables_ = nullptr;	// グローバル変数
	Engine::Camera* camera_ = nullptr;						// カメラ
	Engine::Input* input_ = nullptr;						// 入力
};
