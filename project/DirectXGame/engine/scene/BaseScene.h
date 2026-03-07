#pragma once
#include "Fade.h"
#include <DirectXGame/application/scene/SceneData.h>
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"

namespace Engine {
	// 前方宣言
	class Input;
	class SceneManager;
	class EntityManager;
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
		virtual void SetSceneManader(Engine::SceneManager* sceneManager) { this->sceneManager = sceneManager; }
		// DirectX共通クラス設定
		virtual void SetDirectXCommon(Engine::DirectXCommon* dxCommon) { this->dxCommon = dxCommon; }
		// エンティティマネージャー設定
		void SetEntityManager(Engine::EntityManager* entityManager) { this->entityManager = entityManager; }
		// グローバルバリアブル設定
		void SetGlobalVariables(Engine::GlobalVariables* globalVariables) { this->globalVariables = globalVariables; }
		// Input設定
		void SetInput(Engine::Input* input) { this->input = input; }
		// winApp設定
		void SetWinApp(Engine::WinApp* winApp) { this->winApp = winApp; }
		// DirectX共通クラス取得
		virtual Engine::DirectXCommon* GetDxCommon() { return dxCommon; }
		// エンティティマネージャー取得
		Engine::EntityManager* GetEntityManager() { return entityManager; }
		// シーンマネージャー取得
		Engine::SceneManager* GetSceneManager() { return sceneManager; }
		// グローバルバリアブル取得
		GlobalVariables* GetGlobalVariables() { return globalVariables; }
		// Input取得
		Engine::Input* GetInput() { return input; }
		//
		Engine::WinApp* GetWinApp() { return winApp; }
		// カメラ設定
		void SetCamera(Engine::Camera* camera) { this->camera = camera; };
		// カメラ取得
		Engine::Camera* GetCamara() { return camera; }
		// 終了したか
		bool IsInitialized() const { return initialized_; }
		// シーンデータ取得
		SceneData& GetSceneData();

		// 時間取得
		float GetTime() const;


	private:
		// シーンマネージャ(借りてくる)
		Engine::SceneManager* sceneManager = nullptr;

		Engine::DirectXCommon* dxCommon = nullptr;
		Engine::EntityManager* entityManager = nullptr;
		Engine::Input* input = nullptr;
		Engine::WinApp* winApp = nullptr;
		Engine::GlobalVariables* globalVariables = nullptr;
		Engine::Camera* camera = nullptr;
	protected:
		bool initialized_ = false;
	};
}
