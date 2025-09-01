#include "BaseUI.h"

#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void BaseUI::InitSprite(Sprite* sprite, std::string texFile, Vector2 pos, Vector2 size)
{
	sprite->Initialize( entity2DManager_->GetSpriteCommon(), texFile);
	sprite->SetPosition(pos);
	sprite->SetSize(size);
}

void BaseUI::InitUICheckBox(std::string name, Vector2 pos)
{
	// 読み込み済みモデルを検索
	if (uiCheckBox_.contains(name)) {
		return;
	}
	std::unique_ptr<UICheckBox> sprite = std::make_unique<UICheckBox>();
	sprite->Init(entity2DManager_, name);
	sprite->SetInput(input_);
	sprite->SetPos(pos);
	uiCheckBox_.insert(std::make_pair(name, std::move(sprite)));
}

void BaseUI::InitUISlider(std::string name, Vector2 pos)
{
	// 読み込み済みモデルを検索
	if (uiSlider_.contains(name)) {
		return;
	}
	std::unique_ptr<UISlider> sprite = std::make_unique<UISlider>();
	sprite->Init(entity2DManager_, name);
	sprite->SetInput(input_);
	sprite->SetPos(pos);
	uiSlider_.insert(std::make_pair(name, std::move(sprite)));
}

void BaseUI::InitUIMeter(std::string name, Vector2 pos, bool useSprite)
{
	// 読み込み済みモデルを検索
	if (uiMeter_.contains(name)) {
		return;
	}
	std::unique_ptr<UIMeter> sprite = std::make_unique<UIMeter>();
	sprite->SetUseNameSprite(useSprite);
	sprite->Init(entity2DManager_, name);
	sprite->SetInput(input_);
	sprite->SetPos(pos);

	uiMeter_.insert(std::make_pair(name, std::move(sprite)));
}

void BaseUI::InitUIPair(std::string name, Vector2 pos)
{
	// 読み込み済みモデルを検索
	if (uiPair_.contains(name)) {
		return;
	}
	std::unique_ptr<UIPair> sprite = std::make_unique<UIPair>();
	sprite->Init(entity2DManager_, name);
	sprite->SetInput(input_);
	sprite->SetPos(pos);
	uiPair_.insert(std::make_pair(name, std::move(sprite)));
}

void BaseUI::InitUICount(std::string name, Vector2 pos, int instance, bool useSprite)
{
	// 読み込み済みモデルを検索
	if (uiCount_.contains(name)) {
		return;
	}
	std::unique_ptr<UICount> sprite = std::make_unique<UICount>();
	sprite->SetUseNameSprite(useSprite);
	sprite->SetInstance(instance);
	sprite->Init(entity2DManager_, name);
	sprite->SetInput(input_);
	sprite->SetPos(pos);
	uiCount_.insert(std::make_pair(name, std::move(sprite)));
}

void BaseUI::UpdateUIElement(float deltaTime)
{
	for (auto& ui : uiSlider_) {
		ui.second->SetImageLeftTopPosAndRatio(leftTopPos_, ratio_);
		ui.second->Update(deltaTime);
	}

	for (auto& ui : uiCheckBox_) {
		ui.second->SetImageLeftTopPosAndRatio(leftTopPos_,ratio_);
		ui.second->Update(deltaTime);
	}

	for (auto& ui : uiMeter_) {
		ui.second->SetImageLeftTopPosAndRatio(leftTopPos_, ratio_);
		ui.second->Update(deltaTime);
	}
	for (auto& ui : uiPair_) {
		ui.second->SetImageLeftTopPosAndRatio(leftTopPos_, ratio_);
		ui.second->Update(deltaTime);
	}
	for (auto& ui : uiCount_) {
		ui.second->SetImageLeftTopPosAndRatio(leftTopPos_, ratio_);
		ui.second->Update(deltaTime);
	}
}

void BaseUI::DrawUIElement()
{
	if (!uiSlider_.empty()) {
		for (auto& ui : uiSlider_) {
			ui.second->Draw();
		}
	}
	if (!uiCheckBox_.empty()) {
		for (auto& ui : uiCheckBox_) {
			ui.second->Draw();
		}
	}
	if (!uiMeter_.empty()) {
		for (auto& ui : uiMeter_) {
			ui.second->Draw();
		}
	}
	if (!uiPair_.empty()) {
		for (auto& ui : uiPair_) {
			ui.second->Draw();
		}
	}
	if (!uiCount_.empty()) {
		for (auto& ui : uiCount_) {
			ui.second->Draw();
		}
	}


}

UICheckBox* BaseUI::GetUICheckBox(std::string name)
{
	// 読み込み済みモデルを検索
	if (uiCheckBox_.contains(name)) {
		return uiCheckBox_[name].get();
	}
	return nullptr;
}

UISlider* BaseUI::GetUISlider(std::string name)
{
	// 読み込み済みモデルを検索
	if (uiSlider_.contains(name)) {
		return uiSlider_[name].get();
	}
	return nullptr;
}

UIMeter* BaseUI::GetUIMeter(std::string name)
{
	// 読み込み済みモデルを検索
	if (uiMeter_.contains(name)) {
		return uiMeter_[name].get();
	}
	return nullptr;
}

UIPair* BaseUI::GetUIPair(std::string name)
{
	// 読み込み済みモデルを検索
	if (uiPair_.contains(name)) {
		return uiPair_[name].get();
	}
	return nullptr;
}

UICount* BaseUI::GetUICount(std::string name)
{
	// 読み込み済みモデルを検索
	if (uiCount_.contains(name)) {
		return uiCount_[name].get();
	}
	return nullptr;
}

const std::map<std::string, std::unique_ptr<UIMeter>>& BaseUI::GetUIMeters() const
{
	return uiMeter_;
}

const std::map<std::string, std::unique_ptr<UICount>>& BaseUI::GetUICounts() const
{
	return uiCount_;
}

