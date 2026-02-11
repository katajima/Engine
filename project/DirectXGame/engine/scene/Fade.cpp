
#include "Fade.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

/// <summary>
/// 初期化
/// </summary>
void Engine::Fade::Initialize(Entity2DManager* entity2DManager) {
	entity2DManager_ = entity2DManager;

	// 背景スプライト初期化
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/Image.dds");
	sprite_->SetSize({ static_cast<float>(WinApp::GetClientWidth()),static_cast<float>(WinApp::GetClientHeight()) });
	// フェード持続時間
	duration_ = 0.0f;
	// 経過時間カウンター
	counter_ = 0.0f;
	baseColor_ = { 0,0,0,1 };
};

/// <summary>
/// 毎フレーム処理
/// </summary>
void Engine::Fade::Update() {
	if (status_ == Status::None) return;

	// 時間更新
	counter_ += 1.0f / 60.0f;


	if (counter_ > duration_) {
		counter_ = duration_;
	}

	float t = counter_ / duration_;
	float alpha = 0;
	switch (status_) {
	case Status::FadeIn:// フェードイン
	{
		alpha = std::clamp(1.0f - t, 0.0f, 1.0f);
		
	} 
	break;
	case Status::FadeOut:// フェードアウト
	{
		alpha = std::clamp(t, 0.0f, 1.0f);
	} 
	break;
	}
	// スプライトサイズ、色更新
	sprite_->SetSize({ static_cast<float>(WinApp::GetClientWidth()),static_cast<float>(WinApp::GetClientHeight()) });
	sprite_->SetColor(Color(baseColor_.r, baseColor_.g, baseColor_.b, alpha));
	sprite_->Update();
	if (t >= 1.0f) { 
		Stop(); 
	};
#ifdef _DEBUG
	ImGui::Begin("UUU");
	ImGui::InputFloat("alpha", &alpha);
	ImGui::End();
#endif // _DEBUG
};

/// <summary>
/// 描画
/// </summary>
void Engine::Fade::Draw() {

};

/// <summary>
/// 描画
/// </summary>
void Engine::Fade::Draw2D() {
	if (status_ == Status::None) {
		return;
	}
	// 描画
	sprite_->Draw();

}
void Engine::Fade::DrawCompulsion2D()
{
	// 描画
	sprite_->Draw();
}

// フェード開始
void Engine::Fade::Start(Status status, float duration, const Color& color) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
	baseColor_ = color; // RGBを保持、AlphaはUpdateで変化
};



// フェード終了判定
bool Engine::Fade::IsFinished() const {
	switch (status_) {
	case Fade::Status::FadeIn:
	case Fade::Status::FadeOut:

		if (counter_ >= duration_) {
			return true;
		}
		else {
			return false;
		}
	}

	return true;
}
void Engine::Fade::SetColorUpdate(Color color)
{
	sprite_->SetColor(color);
	sprite_->Update();
}
void Engine::Fade::SetSize(Vector2 size)
{
	sprite_->SetSize({ static_cast<float>(WinApp::GetClientWidth()),static_cast<float>(WinApp::GetClientHeight()) });
}
;