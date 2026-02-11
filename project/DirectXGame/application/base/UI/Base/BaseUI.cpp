#include "BaseUI.h"

#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void BaseUI::InitSprite(Engine::Sprite* sprite, std::string texFile, Vector2 pos, Vector2 size)
{
	sprite->Initialize(entity2DManager->GetSpriteCommon(), texFile);	// 初期化
	sprite->SetPosition(pos);											// 位置設定
	sprite->SetSize(size);												// サイズ設定
}

void BaseUI::InitUICheckBox(std::string name, Vector2 pos)
{
	// 読み込み済みモデルを検索
	if (uiCheckBox_.contains(name)) {
		return;
	}
	std::unique_ptr<Engine::UICheckBox> sprite = std::make_unique<Engine::UICheckBox>();
	sprite->Init(entity2DManager, name);	// 初期化
	sprite->SetInput(input);				// 入力設定
	sprite->SetPos(pos);					// 位置設定
	uiCheckBox_.insert(std::make_pair(name, std::move(sprite)));
}

void BaseUI::InitUISlider(std::string name, Vector2 pos)
{
	// 読み込み済みモデルを検索
	if (uiSlider_.contains(name)) {
		return;
	}
	std::unique_ptr<Engine::UISlider> sprite = std::make_unique<Engine::UISlider>();
	sprite->Init(entity2DManager, name);	// 初期化
	sprite->SetInput(input);				// 入力設定
	sprite->SetPos(pos);					// 位置設定
	uiSlider_.insert(std::make_pair(name, std::move(sprite)));
}

void BaseUI::InitUIMeter(std::string name, Vector2 pos, bool useSprite)
{
	// 読み込み済みモデルを検索
	if (uiMeter_.contains(name)) {
		return;
	}
	std::unique_ptr<Engine::UIMeter> sprite = std::make_unique<Engine::UIMeter>();
	sprite->SetUseNameSprite(useSprite);	// 名前スプライトを追加するか
	sprite->Init(entity2DManager, name);	// 初期化
	sprite->SetInput(input);				// 入力設定
	sprite->SetPos(pos);					// 位置設定
	uiMeter_.insert(std::make_pair(name, std::move(sprite)));
}

void BaseUI::InitUIPair(std::string name, Vector2 pos)
{
	// 読み込み済みモデルを検索
	if (uiPair_.contains(name)) {
		return;
	}
	std::unique_ptr<Engine::UIPair> sprite = std::make_unique<Engine::UIPair>();
	sprite->Init(entity2DManager, name);// 初期化
	sprite->SetInput(input);			 // 入力設定
	sprite->SetPos(pos);				 // 位置設定
	uiPair_.insert(std::make_pair(name, std::move(sprite)));
}

void BaseUI::InitUICount(std::string name, Vector2 pos, int instance, bool useSprite)
{
	// 読み込み済みモデルを検索
	if (uiCount_.contains(name)) {
		return;
	}
	std::unique_ptr<Engine::UICount> sprite = std::make_unique<Engine::UICount>();
	sprite->SetUseNameSprite(useSprite);	// 名前スプライトを使うか
	sprite->SetInstance(instance);			// 桁数
	sprite->Init(entity2DManager, name);	// 初期化
	sprite->SetInput(input);				// 入力設定
	sprite->SetPos(pos);					// 位置設定
	uiCount_.insert(std::make_pair(name, std::move(sprite)));
}

void BaseUI::UpdateUIElement(float deltaTime)
{
	// スライダー更新
	for (auto& ui : uiSlider_) {
		ui.second->SetImageLeftTopPosAndRatio(leftTopPos_, ratio_);
		ui.second->Update(deltaTime);
	}

	// チェックボックス更新
	for (auto& ui : uiCheckBox_) {
		ui.second->SetImageLeftTopPosAndRatio(leftTopPos_,ratio_);
		ui.second->Update(deltaTime);
	}
	// メータ更新
	for (auto& ui : uiMeter_) {
		ui.second->SetImageLeftTopPosAndRatio(leftTopPos_, ratio_);
		ui.second->Update(deltaTime);
	}
	// ペア更新
	for (auto& ui : uiPair_) {
		ui.second->SetImageLeftTopPosAndRatio(leftTopPos_, ratio_);
		ui.second->Update(deltaTime);
	}
	// カウンター更新
	for (auto& ui : uiCount_) {
		ui.second->SetImageLeftTopPosAndRatio(leftTopPos_, ratio_);
		ui.second->Update(deltaTime);
	}
}

void BaseUI::DrawUIElement()
{
	// スライダー描画
	if (!uiSlider_.empty()) {
		for (auto& ui : uiSlider_) {
			ui.second->Draw();
		}
	}
	// チェックボックス描画
	if (!uiCheckBox_.empty()) {
		for (auto& ui : uiCheckBox_) {
			ui.second->Draw();
		}
	}
	// メータ描画
	if (!uiMeter_.empty()) {
		for (auto& ui : uiMeter_) {
			ui.second->Draw();
		}
	}
	// ペア描画
	if (!uiPair_.empty()) {
		for (auto& ui : uiPair_) {
			ui.second->Draw();
		}
	}
	// カウンター描画
	if (!uiCount_.empty()) {
		for (auto& ui : uiCount_) {
			ui.second->Draw();
		}
	}


}

Engine::UICheckBox* BaseUI::GetUICheckBox(std::string name)
{
	// 読み込み済みモデルを検索
	if (uiCheckBox_.contains(name)) {
		return uiCheckBox_[name].get();
	}
	return nullptr;
}

Engine::UISlider* BaseUI::GetUISlider(std::string name)
{
	// 読み込み済みモデルを検索
	if (uiSlider_.contains(name)) {
		return uiSlider_[name].get();
	}
	return nullptr;
}

Engine::UIMeter* BaseUI::GetUIMeter(std::string name)
{
	// 読み込み済みモデルを検索
	if (uiMeter_.contains(name)) {
		return uiMeter_[name].get();
	}
	return nullptr;
}

Engine::UIPair* BaseUI::GetUIPair(std::string name)
{
	// 読み込み済みモデルを検索
	if (uiPair_.contains(name)) {
		return uiPair_[name].get();
	}
	return nullptr;
}

Engine::UICount* BaseUI::GetUICount(std::string name)
{
	// 読み込み済みモデルを検索
	if (uiCount_.contains(name)) {
		return uiCount_[name].get();
	}
	return nullptr;
}

