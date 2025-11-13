#include "UIBoard.h"




void UIBaseBoard::Init(Input* input,Entity2DManager* entity2DManager, const std::string& name, Vector2 pos, const Vector2& size, bool isStatic, std::string textureName)
{
	entity2DManager_ = entity2DManager;		// エンティティ2d
	name_ = name;							// 名前設定
	size_ = size;							// サイズ設定
	isStatic_ = isStatic;					// 静的か設定
	textureName_ = textureName;				// テクスチャ名設定
	input_ = input;							// インプット設定

	// バースプライト初期化
	spriteBar_ = std::make_unique<BaseSprite>();
	spriteBar_->Init(entity2DManager_, name_, textureName_);
	spriteBar_->SetSize({ size.x,30 });				// サイズ設定
	spriteBar_->SetPos(pos);						// 位置設定
	spriteBar_->SetColor({ 0.5f,0.5f ,0.5f,1.0f });	// 色指定

	// ボードスプライト設定
	spriteBoard_ = std::make_unique<BaseSprite>();
	spriteBoard_->Init(entity2DManager_, name_, textureName_);
	spriteBoard_->SetSize(size_);					// サイズ設定
	spriteBoard_->SetPos({ 0 , 30 });				// 位置設定
	spriteBoard_->SetColor({ 0.2f,0.2f, 0.2f, 0.5f });// 色指定

	// 子ども設定
	spriteBar_->GetSprite()->GetWorldTransform2d().SetChild(&spriteBoard_->GetSprite()->GetWorldTransform2d());

}

void UIBaseBoard::Update(float deltaTime)
{
#ifdef _DEBUG
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
#endif // _DEBUG

	// ボードを使用するなら
	if (useBoard_) {
		spriteBar_->Update();
		spriteBoard_->Update();
	}


	// 各機能UI更新
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

	// ボードを描画
	if (useBoard_) {
		spriteBar_->Draw();
		spriteBoard_->Draw();
	}

	// 各機能UI更新
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

	// 各UI機能の初期化
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

	sprite->SetInput(input_);				// インプット
	sprite->SetPos(pos);					// 位置設定
	sprite->SetInstance(instance);			// 桁数設定
	sprite->SetUseNameSprite(useSprite);	// 次のスプライト使うか
	sprite->SetParent(&spriteBoard_->GetSprite()->GetWorldTransform2d());// ボードと親子付け
	sprite->Init(entity2DManager_, name);								 // 初期化
	uiElement_[type].insert(std::make_pair(name, std::move(sprite)));	 //	追加
}


