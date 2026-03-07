#pragma once
#include <iostream>
#include <algorithm>
#include"memory"
#include"DirectXGame/engine/2d/Sprite.h"

namespace Engine {
	// シーンフェーズ
	enum class Phase {
		kFadeIn,	// フェードイン
		kMain,		// メイン部分
		kFadeOut,	// フェードアウト
		kLoading,      // フェードアウト後、シーン生成・Initialize中（画面は黒）
	};

	// 前方宣言
	class EntityManager;

	/// <summary>
	/// フェードクラス
	/// </summary>
	class Fade {
	public:
		enum class Status {
			None,    // フェードなし
			FadeIn,  // フェードイン中
			FadeOut, // フェードアウト中
		};


		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(EntityManager* entityManager);

		/// <summary>
		/// 毎フレーム処理
		/// </summary>
		void Update();

		/// <summary>
		/// 描画
		/// </summary>
		void Draw();

		/// <summary>
		/// 描画
		/// </summary>
		void Draw2D();

		/// <summary>
		/// 強制で描画
		/// </summary>
		void DrawCompulsion2D();

		// フェード開始
		void Start(Status status, float duration, const Color& color);

		// フェード終了
		void Stop() { status_ = Status::None; };;

		// フェード終了判定
		bool IsFinished() const;
		// 色設定
		void SetColor(Color color) { baseColor_ = color; }
		// 色設定
		void SetColorUpdate(Color color);
		// サイズ設定
		void SetSize(Vector2 size);

	private:
		std::unique_ptr<Sprite> sprite_ = nullptr;

		// フェードの状態

		Status status_ = Status::None;

		// フェード持続時間
		float duration_ = 0.0f;
		// 経過時間カウンター
		float counter_ = 0.0f;

		Color baseColor_ = { 0,0,0,1 };
	private:
		EntityManager* entityManager = nullptr;

	};
}

