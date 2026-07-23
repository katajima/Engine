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
	class AudioManager;

	/// <summary>
	/// シーン基底クラス「
	/// </summary>
	class BaseScene
	{
	public:
		virtual ~BaseScene() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		virtual void Initialize() = 0;

		/// <summary>
		/// 終了
		/// </summary>
		virtual void Finalize() = 0;

		/// <summary>
		/// 毎フレーム更新
		/// </summary>
		virtual void Update() = 0;

		// 
		void AllUpdate();

		/// <summary>
		/// 3D描画
		/// </summary>
		virtual void Draw3D() = 0;
		/// <summary>
		/// 2D描画
		/// </summary>
		virtual void Draw2D() = 0;
		/// <summary>
		/// 2D前景描画
		/// </summary>
		virtual void DrawForeground2D() {};
		/// <summary>
		/// 2D全体描画
		/// </summary>
		void AllDraw2D();
		/// <summary>
		/// シーンマネージャー設定
		/// </summary>
		virtual void SetSceneManader(Engine::SceneManager* sceneManager) { this->sceneManager = sceneManager; }
		/// <summary>
		/// DirectX共通クラス設定
		/// </summary>
		virtual void SetDirectXCommon(Engine::DirectXCommon* dxCommon) { this->dxCommon = dxCommon; }
		/// <summary>
		/// エンティティマネージャー設定
		/// </summary>
		void SetEntityManager(Engine::EntityManager* entityManager) { this->entityManager = entityManager; }
		/// <summary>
		/// グローバルバリアブル設定
		/// </summary>
		void SetGlobalVariables(Engine::GlobalVariables* globalVariables) { this->globalVariables = globalVariables; }
		/// <summary>
		/// Input設定
		/// </summary>
		void SetInput(Engine::Input* input) { this->input = input; }
		/// <summary>
		/// winApp設定
		/// </summary>
		void SetWinApp(Engine::WinApp* winApp) { this->winApp = winApp; }
		/// <summary>
		/// オーディオマネージャー設定
		/// </summary>
		void SetAudioManager(Engine::AudioManager* audioManager) { this->audioManager = audioManager; }
		/// <summary>
		/// DirectX共通クラス取得
		/// </summary>
		virtual Engine::DirectXCommon* GetDxCommon() { return dxCommon; }
		/// <summary>
		/// エンティティマネージャー取得
		/// </summary>
		Engine::EntityManager* GetEntityManager() { return entityManager; }
		/// <summary>
		/// シーンマネージャー取得
		/// </summary>
		Engine::SceneManager* GetSceneManager() { return sceneManager; }
		/// <summary>
		/// グローバルバリアブル取得
		/// </summary>
		GlobalVariables* GetGlobalVariables() { return globalVariables; }
		/// <summary>
		/// Input取得
		/// </summary>
		Engine::Input* GetInput() { return input; }
		//
		Engine::WinApp* GetWinApp() { return winApp; }
		/// <summary>
		/// オーディオマネージャー取得
		/// </summary>
		Engine::AudioManager* GetAudioManager() { return audioManager; }
		/// <summary>
		/// カメラ設定
		/// </summary>
		void SetCamera(Engine::Camera* camera) { this->camera = camera; };
		/// <summary>
		/// カメラ取得
		/// </summary>
		Engine::Camera* GetCamara() { return camera; }
		/// <summary>
		/// 終了したか
		/// </summary>
		bool IsInitialized() const { return initialized_; }
		/// <summary>
		/// 初期化状態設定
		/// </summary>
		void SetInitialized(bool initialized) { initialized_ = initialized; }
		/// <summary>
		/// シーンデータ取得
		/// </summary>
		SceneData& GetSceneData();

		/// <summary>
		/// 時間取得
		/// </summary>
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
		Engine::AudioManager* audioManager = nullptr;
	protected:
		bool initialized_ = false;
	};
}
