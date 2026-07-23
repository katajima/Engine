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
		/// <summary>
		/// 初期化
		/// </summary>
		void Init();
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 2d描画
		/// </summary>
		void Draw2D();
		/// <summary>
		/// 3d描画
		/// </summary>
		void Draw3D();

		/// <summary>
		/// 背面2d描画
		/// </summary>
		void DrawForeground2D();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~SceneManager();
		/// <summary>
		/// 終了処理
		/// </summary>
		void Finalize();

		void SetSceneFactory(AbstractSceneFactory* sceneFactory) { this->sceneFactory = sceneFactory; };
		/// <summary>
		/// DirectX共通クラス設定
		/// </summary>
		void SetDirectXCommon(DirectXCommon* directXCommon) { dxCommon = directXCommon; }
		/// <summary>
		/// DirectX共通クラス設定
		/// </summary>
		void SetWinApp(WinApp* winApp) { this->winApp = winApp; }
		/// <summary>
		/// エンティティマネージャー設定
		/// </summary>
		void SetEntityManager(EntityManager* entity3DManager) { this->entityManager = entity3DManager; }
		/// <summary>
		/// グローバルバリアブル設定
		/// </summary>
		void SetGlobalVariables(GlobalVariables* globalVariables) { this->globalVariables = globalVariables; }
		/// <summary>
		/// DirectX共通クラス取得
		/// </summary>
		DirectXCommon* GetDirectXCommon() { return dxCommon; };
		/// <summary>
		/// エンティティ3dマネージャー取得
		/// </summary>
		EntityManager* GetEntityManager() { return entityManager; }
		/// <summary>
		/// グローバルバリアブル取得
		/// </summary>
		GlobalVariables* GetGlobalVariables() { return globalVariables; }
		/// <summary>
		/// Input設定
		/// </summary>
		void SetInput(Input* input) { this->input = input; }
		/// <summary>
		/// オーディオマネージャー設定
		/// </summary>
		void SetAudioManager(AudioManager* audioManager) { this->audioManager = audioManager; }
		/// <summary>
		/// Input取得
		/// </summary>
		Input* GetInput() { return input; }
		/// <summary>
		/// カメラ設定
		/// </summary>
		void SetCamera(Camera* camera) { this->camera = camera; };
		/// <summary>
		/// カメラ取得
		/// </summary>
		Camera* GetCamara() { return camera; }
		/// <summary>
		/// シーンデータ取得
		/// </summary>
		SceneData& GetSceneData() { return sceneData_; }

		/// <summary>
		/// 次のシーン予約
		/// </summary>
		/// <param name="sceneName"></param>
		void ChangeScene(const std::string& sceneName, const float duration = 1.0f);

		/// <summary>
		/// 現在のシーンが何か
		/// </summary>
		bool IsNowScene(std::string name) const {
			if (sceneName == name) {
				return true;
			}
			return false;
		};

		/// <summary>
		/// 終了したか
		/// </summary>
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
