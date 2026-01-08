#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/input/Input.h"


namespace Engine {
	// 前方宣言
	class CameraCommon;

	// デバッグカメラクラス
	class DebugCamera
	{
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(CameraCommon* cameraCommon);

		/// <summary>
		/// 毎フレーム処理
		/// </summary>
		void Update();

		// カメラ取得
		Camera& GetViewProjection() { return camera_; };

	private:
		// ビュープロジェクション
		Camera camera_;

		Input* input_;
	};
}
