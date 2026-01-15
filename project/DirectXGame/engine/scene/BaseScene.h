#pragma once
#include "Fade.h"
#include <DirectXGame/application/scene/SceneData.h>


namespace Engine {
	// 前方宣言
	class Input;
	class SceneManager;
	class Entity3DManager;
	class Entity2DManager;
	class DirectXCommon;
	class GlobalVariables;
	class Camera;

	/// <summary>
	/// シーン基底クラス「
	/// </summary>
	class BaseScene
	{
	public:
		virtual ~BaseScene() = default;

		// 初期化
		virtual void Initialize() = 0;

		// 終了
		virtual void Finalize() = 0;

		// 毎フレーム更新
		virtual void Update() = 0;

		// 
		void AllUpdate();

		// 3D描画
		virtual void Draw3D() = 0;
		// 2D描画
		virtual void Draw2D() = 0;
		// 2D前景描画
		virtual void DrawForeground2D() {};
		// 2D全体描画
		void AllDraw2D();
		// シーンマネージャー設定
		virtual void SetSceneManader(Engine::SceneManager* sceneManager) { sceneManager_ = sceneManager; }
		// DirectX共通クラス設定
		virtual void SetDirectXCommon(Engine::DirectXCommon* dxCommon) { directXCommon_ = dxCommon; }
		// エンティティ3dマネージャー設定
		void SetEntity3DManager(Engine::Entity3DManager* entity3DManager) { entity3DManager_ = entity3DManager; }
		// エンティティ2dマネージャー設定
		void SetEntity2DManager(Engine::Entity2DManager* entity2DManager) { entity2DManager_ = entity2DManager; }
		// グローバルバリアブル設定
		void SetGlobalVariables(Engine::GlobalVariables* globalVariables) { globalVariables_ = globalVariables; }
		// Input設定
		void SetInput(Engine::Input* input) { input_ = input; }
		// DirectX共通クラス取得
		virtual Engine::DirectXCommon* GetDxCommon() { return directXCommon_; }
		// エンティティ3dマネージャー取得
		Engine::Entity3DManager* GetEntity3DManager() { return entity3DManager_; }
		// エンティティ2dマネージャー取得
		Engine::Entity2DManager* GetEntity2DManager() { return entity2DManager_; }
		// シーンマネージャー取得
		Engine::SceneManager* GetSceneManager() { return sceneManager_; }
		// グローバルバリアブル取得
		GlobalVariables* GetGlobalVariables() { return globalVariables_; }
		// Input取得
		Engine::Input* GetInput() { return input_; }
		// カメラ設定
		void SetCamera(Engine::Camera* camera) { camera_ = camera; };
		// カメラ取得
		Engine::Camera* GetCamara() { return camera_; }
		// 終了したか
		bool IsInitialized() const { return initialized_; }
		// シーンデータ取得
		SceneData& GetSceneData();

		// 時間取得
		float GetTime() const;


	private:
		// シーンマネージャ(借りてくる)
		Engine::SceneManager* sceneManager_ = nullptr;

		Engine::DirectXCommon* directXCommon_ = nullptr;
		Engine::Entity3DManager* entity3DManager_;
		Engine::Entity2DManager* entity2DManager_;
		Engine::Input* input_;
		Engine::GlobalVariables* globalVariables_;
		Engine::Camera* camera_;
	protected:
		bool initialized_ = false;
	};
}
