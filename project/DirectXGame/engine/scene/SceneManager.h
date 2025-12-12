#pragma once
#include"BaseScene.h"
#include"AbstractSceneFactory.h"

#include <algorithm> // clamp 用


namespace Engine {
	// 前方宣言
	class Input;
	class DirectXCommon;
	class Entity3DManager;
	class Entity2DManager;
	class GlobalVariables;
	class Camera;

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

		void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; };
		// DirectX共通クラス設定
		void SetDirectXCommon(DirectXCommon* directXCommon) { directXCommon_ = directXCommon; }
		// エンティティ3dマネージャー設定
		void SetEntity3DManager(Entity3DManager* entity3DManager) { entity3DManager_ = entity3DManager; }
		// エンティティ2dマネージャー設定
		void SetEntity2DManager(Entity2DManager* entity2DManager) { entity2DManager_ = entity2DManager; }
		// グローバルバリアブル設定
		void SetGlobalVariables(GlobalVariables* globalVariables) { globalVariables_ = globalVariables; }
		// DirectX共通クラス取得
		DirectXCommon* GetDirectXCommon() { return directXCommon_; };
		// エンティティ3dマネージャー取得
		Entity3DManager* GetEntity3DManager() { return entity3DManager_; }
		// グローバルバリアブル取得
		GlobalVariables* GetGlobalVariables() { return globalVariables_; }
		// Input設定
		void SetInput(Input* input) { input_ = input; }
		// Input取得
		Input* GetInput() { return input_; }
		// カメラ設定
		void SetCamera(Camera* camera) { camera_ = camera; };
		// カメラ取得
		Camera* GetCamara() { return camera_; }
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
		// シーンファクトリー (借りてくる)
		AbstractSceneFactory* sceneFactory_ = nullptr;
		DirectXCommon* directXCommon_;
		Entity3DManager* entity3DManager_;
		Entity2DManager* entity2DManager_;
		Input* input_;
		GlobalVariables* globalVariables_;
		Camera* camera_;


		SceneData sceneData_;

		// 終了フラグ
		bool                  finished_ = false;
		std::unique_ptr<Fade> fade_;
		Phase                 phase_ = Phase::kMain;
	};
}
