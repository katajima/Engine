#pragma once
#include"BaseScene.h"
#include"AbstractSceneFactory.h"

#include <algorithm> // clamp 用


namespace Engine {
	// 前方宣言
	class Input;
	class WinApp;
	class DirectXCommon;
	class EntityManager;
	class GlobalVariables;
	class Camera;
	class AudioManager;

	/// <summary>
	/// シーンマネージャークラス
	/// </summary>
	class SceneManager
	{
	public:
		// 初期化
		void Init();
		// 更新
		void Update();
		// 2d描画
		void Draw2D();
		// 3d描画
		void Draw3D();

		// 背面2d描画
		void DrawForeground2D();

		// デストラクタ
		~SceneManager();
		// 終了処理
		void Finalize();

		void SetSceneFactory(AbstractSceneFactory* sceneFactory) { this->sceneFactory = sceneFactory; };
		// DirectX共通クラス設定
		void SetDirectXCommon(DirectXCommon* directXCommon) { dxCommon = directXCommon; }
		// DirectX共通クラス設定
		void SetWinApp(WinApp* winApp) { this->winApp = winApp; }
		// エンティティマネージャー設定
		void SetEntityManager(EntityManager* entity3DManager) { this->entityManager = entity3DManager; }
		// グローバルバリアブル設定
		void SetGlobalVariables(GlobalVariables* globalVariables) { this->globalVariables = globalVariables; }
		// DirectX共通クラス取得
		DirectXCommon* GetDirectXCommon() { return dxCommon; };
		// エンティティ3dマネージャー取得
		EntityManager* GetEntityManager() { return entityManager; }
		// グローバルバリアブル取得
		GlobalVariables* GetGlobalVariables() { return globalVariables; }
		// Input設定
		void SetInput(Input* input) { this->input = input; }
		// オーディオマネージャー設定
		void SetAudioManager(AudioManager* audioManager) { this->audioManager = audioManager; }
		// Input取得
		Input* GetInput() { return input; }
		// カメラ設定
		void SetCamera(Camera* camera) { this->camera = camera; };
		// カメラ取得
		Camera* GetCamara() { return camera; }
		// シーンデータ取得
		SceneData& GetSceneData() { return sceneData_; }

		/// <summary>
		/// 次のシーン予約
		/// </summary>
		/// <param name="sceneName"></param>
		void ChangeScene(const std::string& sceneName, const float duration = 1.0f);

		// 現在のシーンが何か
		bool IsNowScene(std::string name) const {
			if (sceneName == name) {
				return true;
			}
			return false;
		};

		// 終了したか
		bool IsFinished() const { return finished_; }


	private:
		// 今のシーン(実行中シーン)
		std::unique_ptr<BaseScene> scene_ = nullptr;
		// 次のシーン
		std::unique_ptr<BaseScene> nextScene_ = nullptr;

		std::string sceneName = "";
		
		SceneData sceneData_;

		// 終了フラグ
		bool                  finished_ = false;
		std::unique_ptr<Fade> fade_;
		Phase                 phase_ = Phase::kMain;
	private:
		// シーンファクトリー (借りてくる)
		AbstractSceneFactory* sceneFactory = nullptr;
		DirectXCommon* dxCommon = nullptr;
		EntityManager* entityManager = nullptr;
		Input* input = nullptr;
		GlobalVariables* globalVariables = nullptr;
		Camera* camera = nullptr;
		AudioManager* audioManager = nullptr;
		WinApp* winApp = nullptr;
	};
}
