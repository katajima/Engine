
#include "Fade.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

/// <summary>
/// 初期化
/// </summary>
void Fade::Initialize(Entity2DManager* entity2DManager) {
	entity2DManager_ = entity2DManager;


	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/Image.png");
	//sprite_->Initialize(entity2DManager_->GetSpriteCommon(), "resources/Texture/effect/circle2.png");
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
void Fade::Update() {
	if (status_ == Status::None) return;

	counter_ += 1.0f / 60.0f;

	if (counter_ > duration_) {
		counter_ = duration_;
	}

	float t = counter_ / duration_;
	float alpha = 0;
	switch (status_) {
	case Status::FadeIn: 
	{
		alpha = std::clamp(1.0f - t, 0.0f, 1.0f);
		
	} 
	break;
	case Status::FadeOut:
	{
		alpha = std::clamp(t, 0.0f, 1.0f);
	} 
	break;
	}
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
void Fade::Draw() {

};

/// <summary>
/// 描画
/// </summary>
void Fade::Draw2D() {
	if (status_ == Status::None) {
		return;
	}

	sprite_->Draw();

}
void Fade::DrawCompulsion2D()
{
	sprite_->Draw();
}

// フェード開始
void Fade::Start(Status status, float duration, const Color& color) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
	baseColor_ = color; // RGBを保持、AlphaはUpdateで変化
};



// フェード終了判定
bool Fade::IsFinished() const {
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
void Fade::SetColorUpdate(Color color)
{
	sprite_->SetColor(color);
	sprite_->Update();
}
void Fade::SetSize(Vector2 size)
{
	sprite_->SetSize({ static_cast<float>(WinApp::GetClientWidth()),static_cast<float>(WinApp::GetClientHeight()) });
}
;