#include "UIBoard.h"




void UIBaseBoard::Init(Input* input,Entity2DManager* entity2DManager, const std::string& name, Vector2 pos, const Vector2& size, bool isStatic, std::string textureName)
{
	entity2DManager_ = entity2DManager;
	name_ = name;
	size_ = size;
	isStatic_ = isStatic;
	textureName_ = textureName;
	input_ = input;

	spriteBar_ = std::make_unique<BaseSprite>();
	spriteBar_->Init(entity2DManager_, name_, textureName_);
	spriteBar_->SetSize({ size.x,30 });
	spriteBar_->SetPos(pos);
	spriteBar_->SetColor({ 0.5f,0.5f ,0.5f,1.0f });
	spriteBoard_ = std::make_unique<BaseSprite>();
	spriteBoard_->Init(entity2DManager_, name_, textureName_);
	spriteBoard_->SetSize(size_);
	spriteBoard_->SetPos({ 0 , 30 });
	spriteBoard_->SetColor({0.2f,0.2f, 0.2f, 0.5f});

	spriteBar_->GetSprite()->GetWorldTransform2d().SetChild(&spriteBoard_->GetSprite()->GetWorldTransform2d());

}

void UIBaseBoard::Update(float deltaTime)
{
	ImGui::Begin("UI");
	Vector2 pos = spriteBar_->GetPos();
	ImGui::InputFloat2("bar_pos", &pos.x);

	pos = spriteBoard_->GetPos();
	ImGui::InputFloat2("board_pos", &pos.x);

	pos = spriteBoard_->GetSprite()->GetWorldTransform2d().worldMat_.GetWorldPosition();
	ImGui::InputFloat2("board_posW", &pos.x);

	ImGui::DragFloat2("translate_", &spriteBar_->GetSprite()->GetWorldTransform2d().translate_.x);

	ImGui::DragFloat2("scale_", &spriteBar_->GetSprite()->GetWorldTransform2d().scale_.x, 0.1f);

	ImGui::DragFloat("rotate_", &spriteBar_->GetSprite()->GetWorldTransform2d().rotate_, 0.01f);



	ImGui::End();



	if (useBoard_) {
		spriteBar_->Update();
		spriteBoard_->Update();
	}


	for (auto& [layer, elements] : uiElement_)
	{
		for (auto& [name, element] : elements)
		{
			if (element) {
				element->SetImageLeftTopPosAndRatio(leftTopPos_, ratio_);
				element->Update(deltaTime);
			}
		}
	}


}

void UIBaseBoard::Draw() {
	if (useBoard_) {
		spriteBar_->Draw();
		spriteBoard_->Draw();
	}

	for (auto& [layer, elements] : uiElement_)
	{
		for (auto& [name, element] : elements)
		{
			// 例: UIElement の関数を呼び出す
			if (element) {
				element->Draw();
			}
		}
	}


}

void UIBaseBoard::CreateUIElement(UIType type, std::string name, Vector2 pos, int instance, bool useSprite)
{

	std::unique_ptr<UIElement> sprite;

	if (uiElement_[type].contains(name)) {
		return;
	}

	switch (type)
	{
	case UIType::Normal:
		sprite = std::make_unique<UINormal>();
		break;
	case UIType::Button:
		sprite = std::make_unique<UIButton>();
		break;
	case UIType::CheckBox:
		sprite = std::make_unique<UICheckBox>();
		break;
	case UIType::UISlider:
		sprite = std::make_unique<UISlider>();
		break;
	case UIType::UIMeter:
		sprite = std::make_unique<UIMeter>();
		break;
	case UIType::UIPair:
		sprite = std::make_unique<UIPair>();
		break;
	case UIType::UICount:
		sprite = std::make_unique<UICount>();
		break;
	default:
		return;
		break;
	}

	sprite->SetInput(input_);
	sprite->SetPos(pos);
	sprite->SetInstance(instance);
	sprite->SetUseNameSprite(useSprite);
	sprite->SetParent(&spriteBoard_->GetSprite()->GetWorldTransform2d());
	sprite->Init(entity2DManager_, name);
	uiElement_[type].insert(std::make_pair(name, std::move(sprite)));
}


