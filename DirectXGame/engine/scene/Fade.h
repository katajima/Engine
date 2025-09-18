#pragma once
#include <iostream>
#include <algorithm>
#include"memory"
#include"DirectXGame/engine/2d/Sprite.h"


// シーンフェーズ
enum class Phase {
	kFadeIn,	// フェードイン
	kMain,		// メイン部分
	kFadeOut,	// フェードアウト
	kLoading,      // フェードアウト後、シーン生成・Initialize中（画面は黒）
};

class Entity2DManager;
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
	void Initialize(Entity2DManager* entity2DManager);

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


	void DrawCompulsion2D();

	// フェード開始
	void Start(Status status, float duration ,const Color& color);

	// フェード終了
	void Stop(){ status_ = Status::None; };;

	// フェード終了判定
	bool IsFinished() const;

	void SetColor(Color color) { baseColor_ = color; }

	void SetColorUpdate(Color color);

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
	Entity2DManager* entity2DManager_;

};


