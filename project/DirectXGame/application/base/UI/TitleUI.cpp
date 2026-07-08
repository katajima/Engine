#include "TitleUI.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include <DirectXGame/engine/Utility/ConvertUtility.h>

namespace {
	// タイトル決定ボタンの表示位置とサイズ。
	const Vector2 kDecisionIconPosition = { 640.0f, 600.0f };
	const Vector2 kDecisionIconSize = { 200.0f, 200.0f };
	// UI画像を中央基準で配置するアンカー。
	const Vector2 kCenterAnchor = { 0.5f, 0.5f };
	// タイトル画像へ設定する明度。
	constexpr float kTitleColorIntensity = 0.75f;
	// 決定後にタイトル画像を上へ移動する速度。
	constexpr float kTitleExitMoveSpeed = 300.0f;
	// 決定アイコンを点滅させる1フレーム当たりの透明度変化量。
	constexpr float kButtonAlphaStepPerFrame = 0.015f;
	// UI透明度の有効範囲。
	constexpr float kMinAlpha = 0.0f;
	constexpr float kMaxAlpha = 1.0f;
}

void TitleUI::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables) {
	iconB_ = std::make_unique<Engine::Sprite>();
	iconB_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/icon/B.dds");
	iconB_->SetPosition(kDecisionIconPosition);
	iconB_->SetAnchorPoint(kCenterAnchor);
	iconB_->SetSize(kDecisionIconSize);

	title_ = std::make_unique<Engine::Sprite>();
	title_->Initialize(entityManager->GetSpriteCommon(), "resources/Texture/text/title.dds");
	title_->SetPosition(titlePos_);
	title_->SetAnchorPoint(kCenterAnchor);
	title_->SetSize(2);
	title_->SetColor({ kTitleColorIntensity, kTitleColorIntensity, kTitleColorIntensity, kMaxAlpha });
}

void TitleUI::Update(float dt) {
	// ボタンUI処理
	ButtonUiProcess(dt);
	// タイトルUI処理
	TitleUiProcess(dt);
}

void TitleUI::Draw() {
	iconB_->Draw();
	title_->Draw();
}

void TitleUI::ButtonUiProcess(float dt) {

	if (isAction_) {
		titlePos_.y -= kTitleExitMoveSpeed * dt;

		title_->SetPosition(titlePos_);

		if (buttonColor_.a <= kMinAlpha) {
			return;
		}
	}

	if (buttonColor_.a >= kMaxAlpha) {
		buttonColor_.a = kMaxAlpha;
		buttonClock_ *= -1;
	}
	else if (buttonColor_.a <= kMinAlpha) {
		buttonColor_.a = kMinAlpha;
		buttonClock_ *= -1;
	}

	if (buttonClock_ == 1) {
		buttonColor_.a += kButtonAlphaStepPerFrame;
	}
	else {
		buttonColor_.a -= kButtonAlphaStepPerFrame;
	}

	// 色設定
	iconB_->SetColor(buttonColor_);
	// UI更新
	iconB_->Update();
}

void TitleUI::TitleUiProcess(float dt) {
	// タイトルUI更新
	title_->Update();
}
