#pragma once
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/input/Input.h"
#include <DirectXGame/engine/Audio/Audio.h>
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include"DirectXGame/engine/Manager/Entity/EntityManager.h"
#include"DirectXGame/engine/3d/Model/ModelCommon.h"
#include "DirectXGame/engine/Offscreen/RenderingCommon.h"

#include"DirectXGame/engine/scene/SceneManager.h"
#include"DirectXGame/engine/scene/AbstractSceneFactory.h"
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"

// C++
#include"memory"


namespace Engine {
	// ゲーム全体
/// <summary>
/// Frameworkを管理・実装するクラス。
/// </summary>
	class Framework
	{
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		virtual void Initialize();

		/// <summary>
		/// 終了
		/// </summary>
		virtual void Finalize();

		/// <summary>
		/// 毎フレーム更新
		/// </summary>
		virtual void Update();

		/// <summary>
		/// 描画
		/// </summary>
		virtual void Draw() = 0;

		/// <summary>
		/// 終了フラグのチェック
		/// </summary>
		virtual bool IsEndRequest() { return endRequest_; }

		/// <summary>
		/// デストラクタ
		/// </summary>
		virtual ~Framework() = default;

		/// <summary>
		/// 実行
		/// </summary>
		void Run();

	protected:
		// ゲーム終了フラグ
		bool endRequest_ = false;
		// ウィンアップ

		std::unique_ptr <WinApp> winApp_ = nullptr;

		// 入力
		std::unique_ptr<Input> input_ = nullptr;
		// 音
		std::unique_ptr<AudioManager> audioManager_ = nullptr;

		// ダイレクトX
		std::unique_ptr <DirectXCommon> dxCommon_ = nullptr;

		// エンティティ全般
		std::unique_ptr<EntityManager> entityManager_ = nullptr;


		// シーンマネージャー
		std::unique_ptr<SceneManager> sceneManager_ = nullptr;

		// シーンファクトリー
		std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;

		// グローバル
		std::unique_ptr<GlobalVariables> globalVariables_ = nullptr;

	};
}
