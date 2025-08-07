#include "UIElement.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"


static Vector2 ToLocalSpace(const Vector2& worldPos, const WorldTransform2d& parentTransform) {
	Matrix3x3 invMat = Inverse(parentTransform.worldMat_);
	return Transforms(worldPos, invMat);
}

void UIElement::Init(Entity2DManager* entity2DManager, std::string name)
{
	entity2DManager_ = entity2DManager;
	mainName_ = name;

	InitSprite();

#ifdef _DEBUG
	isDebuck_ = true;
#endif // _DEBUG


}

void UIElement::AddSprite(std::string name, std::string textureName)
{
	// 読み込み済みモデルを検索
	if (sprites_.contains(name)) {
		return;
	}
	std::unique_ptr<BaseSprite> sprite = std::make_unique<BaseSprite>();
	sprite->Init(entity2DManager_, name, textureName);

	sprites_.insert(std::make_pair(name, std::move(sprite)));
}

BaseSprite* UIElement::GetSprite(std::string name)
{
	if (sprites_.contains(name)) {
		return sprites_[name].get();
	}
	else {
		return nullptr;
	}
}

void UIElement::Draw()
{
	for (auto& sprit : sprites_) {
		sprit.second->Update();
		sprit.second->Draw();
	}

	UniqueDraw();
}


#pragma region UINormal

void UINormal::InitSprite() {}

void UINormal::Update(float deltaTime) {}

#pragma endregion

#pragma region UIButton

void UIButton::InitSprite(){}

void UIButton::Update(float deltaTime) {


}

#pragma endregion

#pragma region UICheckBox

void UICheckBox::InitSprite()
{
	checkSprite = std::make_unique<BaseSprite>();
	checkSprite->Init(entity2DManager_, "check", "resources/Texture/Image.png");
	checkSprite->GetSprite()->SetSize({24.0f,24.0f});
	checkSprite->GetSprite()->SetAnchorPoint({0.5f,0.5f});
	backgroundSprite = std::make_unique<BaseSprite>();
	backgroundSprite->Init(entity2DManager_, "background", "resources/Texture/Image.png");
	backgroundSprite->GetSprite()->SetAnchorPoint({ 0.5f,0.5f });
	backgroundSprite->GetSprite()->SetColor({0.5f,0.5f ,0.5f ,1.0f});
	backgroundSprite->GetSprite()->SetSize({ 30.0f,30.0f });
	backgroundSprite->SetUseColl(true);

	if (parent_) {
		checkSprite->GetSprite()->GetWorldTransform2d().parent_ = parent_;
		backgroundSprite->GetSprite()->GetWorldTransform2d().parent_ = parent_;
	}

}


void UICheckBox::Update(float deltaTime) 
{
	checkSprite->GetSprite()->SetPosition(pos_);
	backgroundSprite->GetSprite()->SetPosition(pos_);

	backgroundSprite->SetImageLeftTopPosAndRatio(leftTopPos_,ratio_);
	if (backgroundSprite->GetBox().intersects(input_->GetMousePosition())) {
		if (input_->IsMouseTriggered(0)){
			if (!isCheck_) {
				isCheck_ = true;
			}
			else {
				isCheck_ = false;
			}
		}
	}
	else {

	}
	checkSprite->Update();
	backgroundSprite->Update();
}

void UICheckBox::UniqueDraw() {
	backgroundSprite->Draw();
	if (isCheck_) {
		checkSprite->Draw();
	}
}

#pragma endregion

#pragma region UISlider

void UISlider::InitSprite() {
	slidSprite = std::make_unique<BaseSprite>();
	slidSprite->Init(entity2DManager_, "slid", "resources/Texture/Image.png");
	slidSprite->GetSprite()->SetSize({ 24.0f,24.0f });
	slidSprite->GetSprite()->SetAnchorPoint({ 0.5f,0.5f });
	slidSprite->SetUseColl(true);
	backgroundSprite = std::make_unique<BaseSprite>();
	backgroundSprite->Init(entity2DManager_, "background", "resources/Texture/Image.png");
	backgroundSprite->GetSprite()->SetAnchorPoint({ 0.0f,0.5f });
	backgroundSprite->GetSprite()->SetColor({ 0.5f,0.5f ,0.5f ,1.0f });
	backgroundSprite->GetSprite()->SetSize({ 500.0f,30.0f });
	backgroundSprite->SetUseColl(true);
	backgroundSprite->GetSprite()->GetWorldTransform2d().SetChild(&slidSprite->GetSprite()->GetWorldTransform2d());
	
	if (parent_) {
		backgroundSprite->GetSprite()->GetWorldTransform2d().parent_ = parent_;
	}
}

void UISlider::Update(float deltaTime) {
	backgroundSprite->GetSprite()->SetPosition(pos_);
	backgroundSprite->SetImageLeftTopPosAndRatio(leftTopPos_, ratio_);
	slidSprite->SetImageLeftTopPosAndRatio(leftTopPos_, ratio_);

	preMousePos = input_->GetMousePosition();

	Box box = backgroundSprite->GetBox();
	Vector2 size = backgroundSprite->GetSize();


	Vector2 result = offsetPos_;



	const float margin = 0.0f;

	// スライダーの半サイズ
	float leftHalf = slidSprite->GetHalfSise(SpriteSize::Left).x;
	float rightHalf = slidSprite->GetHalfSise(SpriteSize::Right).x;

	// マウス座標と背景BoxのX範囲
	Vector2 mouse = preMousePos;
	float boxMinX = box.min_.x;
	float boxMaxX = box.max_.x;

	// デバッグ時は比率補正を解除
	if (isDebuck_) {
		mouse.x /= ratio_.x;
		mouse.y/= ratio_.y;
		boxMinX /= ratio_.x;
		boxMaxX /= ratio_.x;
		size.x /= ratio_.x;
	}



	// スライダーのクリック判定
	if (slidSprite->GetBox().intersects(preMousePos)) {
		if (input_->IsMousePressed(0)) {
			isClick = true;
		}
	}
	else {
		if (input_->IsMouseReleased(0)) {
			isClick = false;
		}
	}

	// スライダーの移動処理（背景Boxの範囲に制限）
	if (isClick) {
		// 左・右の制限境界
		float leftLimit = leftHalf * 2;
		float rightLimit = size.x - rightHalf *2;

		//mouse.x /= ratio_.x;

		// マウス座標を背景スプライトのローカル座標に変換
		Vector2 localMousePos = ToLocalSpace(mouse, backgroundSprite->GetSprite()->GetWorldTransform2d());
		float mouseX = localMousePos.x;


		// マウスXをスライダー範囲にクランプ
		mouseX = std::clamp(mouseX, leftLimit, rightLimit);
		
		
		

		
		// デバッグ補正戻す
		if (isDebuck_) {
			result.x = mouseX * ratio_.x;;
		}
		else {
			result.x = mouseX;
		}

		offsetPos_ = result;

	}

	// ImGui表示（任意）
	ImGui::Begin("UI");
	Vector2 pos = slidSprite->GetSprite()->GetWorldTransform2d().GetWorldPosition();
	ImGui::InputFloat2("slidSpritePos", &pos.x);
	ImGui::InputFloat2("preMousePos", &preMousePos.x);
	ImGui::InputFloat("mouseX", &mouse.x);
	ImGui::End();
	offsetPos_.y = 0;
	// スライダー位置更新
	slidSprite->GetSprite()->SetPosition(offsetPos_);
	backgroundSprite->Update();
	slidSprite->Update();
}

void UISlider::UniqueDraw() {
	backgroundSprite->Draw();
	slidSprite->Draw();
}

#pragma endregion

#pragma region UIMeter

void UIMeter::InitSprite() {
	meterSprite = std::make_unique<BaseSprite>();
	meterSprite->Init(entity2DManager_, "slid", "resources/Texture/Image.png");
	meterSprite->GetSprite()->SetSize({ 494.0f,24.0f });
	meterSprite->GetSprite()->SetAnchorPoint({ 0.5f,0.5f });
	backgroundSprite = std::make_unique<BaseSprite>();
	backgroundSprite->Init(entity2DManager_, "background", "resources/Texture/Image.png");
	backgroundSprite->GetSprite()->SetAnchorPoint({ 0.0f,0.5f });
	backgroundSprite->GetSprite()->SetColor({ 0.5f,0.5f ,0.5f ,1.0f });
	backgroundSprite->GetSprite()->SetSize({ 500.0f,30.0f });

	if (useNameSprite_) {
		nameSprite_ = std::make_unique<BaseSprite>();
		nameSprite_->Init(entity2DManager_, "name", "resources/Texture/Image.png");
		nameSprite_->SetAnchorPoint(Vector2(0.0f, 0.5f));
		nameSprite_->SetSize({ 24.0f,24.0f });
	}

	if (parent_) {
		meterSprite->GetSprite()->GetWorldTransform2d().parent_ = parent_;
		backgroundSprite->GetSprite()->GetWorldTransform2d().parent_ = parent_;
		if (useNameSprite_) {
			nameSprite_->GetSprite()->GetWorldTransform2d().parent_ = parent_;
		}
	}

}

void UIMeter::Update(float deltaTime) {
	if (nameSprite_) {
		nameSprite_->GetSprite()->SetPosition(pos_);
		nameSprite_->Update();
		
		meterSprite->GetSprite()->SetPosition(pos_ + Vector2(nameSprite_->GetHalfSise(SpriteSize::Right)));
		backgroundSprite->GetSprite()->SetPosition(pos_ + Vector2(nameSprite_->GetHalfSise(SpriteSize::Right)));
		
	}
	else {
		meterSprite->GetSprite()->SetPosition(pos_);
		backgroundSprite->GetSprite()->SetPosition(pos_);
	}


	// 正規化されたメーター値（0～1）
	float rate = 0.0f;
	if (meterMax_ != meterMin_) {
		rate = std::clamp((meter_ - meterMin_) / (meterMax_ - meterMin_), 0.0f, 1.0f);
	}

	// 基本サイズの再設定
	Vector2 baseSize = size_;
	Vector2 meterSize = baseSize;
	Vector2 meterPos = pos_;

	// アンカーポイントとサイズを方向に応じて調整
	switch (type_) {
	case UIMeterType::Top:
		meterSize.y *= rate;
		meterSprite->GetSprite()->SetAnchorPoint({ 0.0f, 0.0f });
		backgroundSprite->GetSprite()->SetAnchorPoint({ 0.0f, 0.0f });
		meterPos += Vector2(offsetSize.x * 0.5f, offsetSize.x * 0.5f);
		break;
	case UIMeterType::Down:
		meterSize.y *= rate;
		meterSprite->GetSprite()->SetAnchorPoint({ 0.0f, 1.0f });
		backgroundSprite->GetSprite()->SetAnchorPoint({ 0.0f, 1.0f });
		meterPos += Vector2(offsetSize.x * 0.5f, -offsetSize.x * 0.5f);
		break;
	case UIMeterType::Right:
		meterSize.x *= rate;
		meterSprite->GetSprite()->SetAnchorPoint({ 1.0f, 0.5f });
		backgroundSprite->GetSprite()->SetAnchorPoint({ 0.0f, 0.5f });
		meterPos += Vector2(-(offsetSize.x * 0.5f) + backgroundSprite->GetHalfSise(SpriteSize::Right).x, 0);
		break;
	case UIMeterType::Left:
		meterSize.x *= rate;
		meterSprite->GetSprite()->SetAnchorPoint({ 0.0f, 0.5f });
		backgroundSprite->GetSprite()->SetAnchorPoint({ 0.0f, 0.5f });
		meterPos += Vector2(offsetSize.x * 0.5f, 0);
		break;
	case UIMeterType::WidthCenter:
		meterSize.x *= rate;
		meterSprite->GetSprite()->SetAnchorPoint({ 0.5f, 0.5f });
		backgroundSprite->GetSprite()->SetAnchorPoint({ 0.0f , 0.5f });
		meterPos += Vector2(-(offsetSize.x * 0.5f) * 0.5f + backgroundSprite->GetHalfSise(SpriteSize::Right).x * 0.5f, 0);
		break;
	case UIMeterType::HeightCenter:
		meterSize.y *= rate;
		meterSprite->GetSprite()->SetAnchorPoint({ 0.0f, 0.5f });
		backgroundSprite->GetSprite()->SetAnchorPoint({ 0.0f, 0.5f });
		meterPos += Vector2(offsetSize.x * 0.5f, 0);
		break;
	default:
		break;
	}
	Vector2 anchor = meterSprite->GetSprite()->GetAnchorPoint();
	// サイズと位置の反映
	meterSprite->GetSprite()->SetSize(meterSize - offsetSize);
	meterSprite->GetSprite()->SetPosition(meterPos);
	backgroundSprite->GetSprite()->SetSize(baseSize);
	backgroundSprite->GetSprite()->SetPosition(pos_);

	meterSprite->Update();
	backgroundSprite->Update();

	
}

void UIMeter::UniqueDraw() {
	backgroundSprite->Draw();
	meterSprite->Draw();
	if (nameSprite_) {
		nameSprite_->Draw();
	}
}

#pragma endregion

#pragma region UIPair

void UIPair::InitSprite() 
{
	firstSprite = std::make_unique<BaseSprite>();
	firstSprite->Init(entity2DManager_, "first", "resources/Texture/Image.png");
	secondSprite = std::make_unique<BaseSprite>();
	secondSprite->Init(entity2DManager_, "second", "resources/Texture/Image.png");
	if (parent_) {
		firstSprite->GetSprite()->GetWorldTransform2d().parent_ = parent_;
		secondSprite->GetSprite()->GetWorldTransform2d().parent_ = parent_;
	}
	
}

void UIPair::Update(float deltaTime) {
	firstSprite->GetSprite()->SetPosition(pos_);

	Vector2 secondpos = pos_;
	switch (type_)
	{
	case UIPairDrectionType::Top:
		secondpos.y -= offset_ + firstSprite->GetHalfSise(SpriteSize::Top).y;
		break;
	case UIPairDrectionType::Down:
		secondpos.y += offset_ + firstSprite->GetHalfSise(SpriteSize::Down).y;
		break;
	case UIPairDrectionType::Right:
		secondpos.x += offset_ + firstSprite->GetHalfSise(SpriteSize::Right).x;
		break;
	case UIPairDrectionType::Left:
		secondpos.x -= offset_ + firstSprite->GetHalfSise(SpriteSize::Left).x;
		break;
	default:
		break;
	}
	
	
	
	
	secondSprite->GetSprite()->SetPosition(secondpos);

	firstSprite->Update();
	secondSprite->Update();
}

void UIPair::UniqueDraw() {
	firstSprite->Draw();
	secondSprite->Draw();
}

#pragma endregion

#pragma region UICount

void UICount::InitSprite()
{
	for (int i = 0; i < instance_; i++) {
		auto sprite = std::make_unique<BaseSprite>();
		sprite->Init(entity2DManager_, "count", "resources/Texture/num/Number_x64y96.png");
		sprite->SetSize({ 24.0f,24.0f });
		sprite->SetAnchorPoint({ 0.5f,0.5f });

		countSprite_.push_back(std::move(sprite));
	}	
	if (useNameSprite_) {
		nameSprite_ = std::make_unique<BaseSprite>();
		nameSprite_->Init(entity2DManager_, "name", "resources/Texture/Image.png");
		nameSprite_->SetAnchorPoint(Vector2(0.0f, 0.5f));
		nameSprite_->SetSize({ 24.0f,24.0f });
	}

	if (parent_) {
		for (auto& sprite : countSprite_) {
			sprite->GetSprite()->GetWorldTransform2d().parent_ = parent_;
		}
		if (useNameSprite_) {
			nameSprite_->GetSprite()->GetWorldTransform2d().parent_ = parent_;
		}
	}
}


void UICount::Update(float deltaTime)
{
	if (count_ >= countMax_) {
		count_ = countMax_;
	}

	int count = (std::max)(0, static_cast<int>(count_)); // 念のためマイナス防止
	int numDigits = (count == 0) ? 1 : static_cast<int>(log10(static_cast<double>(count))) + 1;

	int i = 0;
	for (auto& sprite : countSprite_) {
		if (i >= numDigits) break; // 桁数を超えたら描画しない

		int digit = (count / static_cast<int>(pow(10, i))) % 10;

		sprite->GetSprite()->SetTextureLeftTop(Vector2((texSize_.x * digit), 0));
		Vector2 pos = pos_ + Vector2((texSize_.x * static_cast<float>((instance_ - 1) - i) * 0.5f), 0.0f) + (offsetSize * static_cast<float>((instance_ - 1) - i));
		sprite->GetSprite()->SetPosition(pos);
		sprite->Update();
		i++;
	}

	if (nameSprite_) {

		nameSprite_->GetSprite()->SetPosition(countSprite_.front()->GetSprite()->GetPosition() + Vector2(offsetSize.x,0) );
		nameSprite_->Update();
	}
}


void UICount::UniqueDraw() {
	for (auto& sprite : countSprite_) {
		sprite->Draw();
	}
	if (nameSprite_) {
		nameSprite_->Draw();
	}
}

#pragma endregion
