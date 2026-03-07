#include "UIElement.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"


static Vector2 ToLocalSpace(const Vector2& worldPos, const Engine::WorldTransform2d& parentTransform) {
	Matrix3x3 invMat = Inverse(parentTransform.worldMat_);
	return Transforms(worldPos, invMat);
}

void Engine::UIElement::Init(EntityManager* entityManager, std::string name)
{
	this->entityManager = entityManager;	// エンティティ管理
	mainName_ = name;					// 名前設定

	// スプライト初期化
	InitSprite();

#ifdef _DEBUG
	isDebuck_ = true;
#endif // _DEBUG


}

void Engine::UIElement::AddSprite(std::string name, std::string textureName)
{
	// 読み込み済みモデルを検索
	if (sprites_.contains(name)) {
		return;
	}
	std::unique_ptr<BaseSprite> sprite = std::make_unique<BaseSprite>();
	sprite->Init(entityManager, name, textureName);

	sprites_.insert(std::make_pair(name, std::move(sprite)));
}

Engine::BaseSprite* Engine::UIElement::GetSprite(std::string name)
{
	// スプライトがあるなら返す
	if (sprites_.contains(name)) {
		return sprites_[name].get();
	}
	else {
		return nullptr;
	}
}

void Engine::UIElement::Draw()
{
	// スプライト更新描画
	for (auto& sprit : sprites_) {
		sprit.second->Update();
		sprit.second->Draw();
	}

	// 固有の描画
	UniqueDraw();
}


#pragma region UINormal

void Engine::UINormal::InitSprite() {}

void Engine::UINormal::Update(float deltaTime) {}

#pragma endregion

#pragma region UIButton

void Engine::UIButton::InitSprite(){}

void Engine::UIButton::Update(float deltaTime) {


}

#pragma endregion

#pragma region UICheckBox

void Engine::UICheckBox::InitSprite()
{
	// チェックボタン初期化
	checkSprite = std::make_unique<BaseSprite>();
	checkSprite->Init(entityManager, "check", "resources/Texture/Image.dds");
	checkSprite->GetSprite()->SetSize({24.0f,24.0f});		// サイズ設定
	checkSprite->GetSprite()->SetAnchorPoint({0.5f,0.5f});	// アンカーポイント設定
	
	// 背景スプライト初期化
	backgroundSprite = std::make_unique<BaseSprite>();
	backgroundSprite->Init(entityManager, "background", "resources/Texture/Image.dds");
	backgroundSprite->GetSprite()->SetAnchorPoint({ 0.5f,0.5f });		// アンカーポイント設定
	backgroundSprite->GetSprite()->SetColor({0.5f,0.5f ,0.5f ,1.0f});	//色指定
	backgroundSprite->GetSprite()->SetSize({ 30.0f,30.0f });			// サイズ設定
	backgroundSprite->SetUseColl(true);									// コライダーを使うか

	// 親子
	if (parent) {
		checkSprite->GetSprite()->GetWorldTransform2d().parent_ = parent;
		backgroundSprite->GetSprite()->GetWorldTransform2d().parent_ = parent;
	}

}


void Engine::UICheckBox::Update(float deltaTime)
{
	// 各位置の更新
	checkSprite->GetSprite()->SetPosition(pos_);
	backgroundSprite->GetSprite()->SetPosition(pos_);

	backgroundSprite->SetImageLeftTopPosAndRatio(leftTopPos_,ratio_);
	
	//// 入っているなら
	//if (backgroundSprite->GetBox().intersects(input_->GetMousePosition())) {
	//	if (input_->IsMouseTriggered(0)){
	//		if (!isCheck_) {
	//			isCheck_ = true;
	//		}
	//		else {
	//			isCheck_ = false;
	//		}
	//	}
	//}
	//else {

	//}
	// 更新
	checkSprite->Update();
	backgroundSprite->Update();
}

void Engine::UICheckBox::UniqueDraw() {
	
	// 描画
	backgroundSprite->Draw();
	if (isCheck_) {
		checkSprite->Draw();
	}
}

#pragma endregion

#pragma region UISlider

void Engine::UISlider::InitSprite() {

	// スライダー初期化
	slidSprite = std::make_unique<BaseSprite>();
	slidSprite->Init(entityManager, "slid", "resources/Texture/Image.dds");
	slidSprite->GetSprite()->SetSize({ 24.0f,24.0f });		// サイズ設定
	slidSprite->GetSprite()->SetAnchorPoint({ 0.5f,0.5f });	// アンカーポイント設定
	slidSprite->SetUseColl(true);							// コライダー使うか設定

	// 背景スプライト初期化
	backgroundSprite = std::make_unique<BaseSprite>();
	backgroundSprite->Init(entityManager, "background", "resources/Texture/Image.dds");
	backgroundSprite->GetSprite()->SetAnchorPoint({ 0.0f,0.5f });		// アンカーポイント設定
	backgroundSprite->GetSprite()->SetColor({ 0.5f,0.5f ,0.5f ,1.0f });	// 色設定
	backgroundSprite->GetSprite()->SetSize({ 500.0f,30.0f });			// サイズ設定
	backgroundSprite->SetUseColl(true);									// コライダー使うか設定
	backgroundSprite->GetSprite()->GetWorldTransform2d().SetChild(&slidSprite->GetSprite()->GetWorldTransform2d());
	
	//	親子付け
	if (parent) {
		backgroundSprite->GetSprite()->GetWorldTransform2d().parent_ = parent;
	}
}

void Engine::UISlider::Update(float deltaTime) {

	// 比率と位置の設定
	backgroundSprite->GetSprite()->SetPosition(pos_);
	backgroundSprite->SetImageLeftTopPosAndRatio(leftTopPos_, ratio_);
	slidSprite->SetImageLeftTopPosAndRatio(leftTopPos_, ratio_);

	//preMousePos = input_->GetMousePosition();

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



	//// スライダーのクリック判定
	//if (slidSprite->GetBox().intersects(preMousePos)) {
	//	if (input_->IsMousePressed(0)) {
	//		isClick = true;
	//	}
	//}
	//else {
	//	if (input_->IsMouseReleased(0)) {
	//		isClick = false;
	//	}
	//}

	// スライダーの移動処理（背景Boxの範囲に制限）
	if (isClick) {
		// 左・右の制限境界
		float leftLimit = leftHalf * 2;
		float rightLimit = size.x - rightHalf *2;

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
#ifdef _DEBUG
	// ImGui表示（任意）
	ImGui::Begin("UI");
	Vector2 pos = slidSprite->GetSprite()->GetWorldTransform2d().GetWorldPosition();
	ImGui::InputFloat2("slidSpritePos", &pos.x);
	ImGui::InputFloat2("preMousePos", &preMousePos.x);
	ImGui::InputFloat("mouseX", &mouse.x);
	ImGui::End();
#endif // _DEBUG
	offsetPos_.y = 0;
	// スライダー位置更新
	slidSprite->GetSprite()->SetPosition(offsetPos_);
	backgroundSprite->Update();
	slidSprite->Update();
}

void Engine::UISlider::UniqueDraw() {
	backgroundSprite->Draw();
	slidSprite->Draw();
}

#pragma endregion

#pragma region UIMeter

void Engine::UIMeter::InitSprite() {
	// メータスプライト初期化
	meterSprite = std::make_unique<BaseSprite>();
	meterSprite->Init(entityManager, "slid", "resources/Texture/Image.dds");
	meterSprite->GetSprite()->SetSize({ 494.0f,24.0f });			// サイズ設定
	meterSprite->GetSprite()->SetAnchorPoint({ 0.5f,0.5f });		// アンカーポイント設定

	// 背景スプライト初期化
	backgroundSprite = std::make_unique<BaseSprite>();				
	backgroundSprite->Init(entityManager, "background", "resources/Texture/Image.dds");
	backgroundSprite->GetSprite()->SetAnchorPoint({ 0.0f,0.5f });		// アンカーポイント設定
	backgroundSprite->GetSprite()->SetColor({ 0.5f,0.5f ,0.5f ,1.0f });	// 色設定
	backgroundSprite->GetSprite()->SetSize({ 500.0f,30.0f });			// サイズ設定

	// 名前スプライトを使うなら
	if (useNameSprite_) {
		nameSprite_ = std::make_unique<BaseSprite>();
		nameSprite_->Init(entityManager, "name", "resources/Texture/Image.dds");
		nameSprite_->SetAnchorPoint(Vector2(0.0f, 0.5f));	// アンカーポイント設定
		nameSprite_->SetSize({ 24.0f,24.0f });				// サイズ設定
	}

	// 親子付け
	if (parent) {
		meterSprite->GetSprite()->GetWorldTransform2d().parent_ = parent;
		backgroundSprite->GetSprite()->GetWorldTransform2d().parent_ = parent;
		if (useNameSprite_) {
			nameSprite_->GetSprite()->GetWorldTransform2d().parent_ = parent;
		}
	}

}

void Engine::UIMeter::Update(float deltaTime) {

	// 名前スプライトがあるなら
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
	case UIMeterType::Top:	// 上方向
		meterSize.y *= rate;
		meterSprite->GetSprite()->SetAnchorPoint({ 0.0f, 0.0f });
		backgroundSprite->GetSprite()->SetAnchorPoint({ 0.0f, 0.0f });
		meterPos += Vector2(offsetSize.x * 0.5f, offsetSize.x * 0.5f);
		break;
	case UIMeterType::Down:	// 下方向
		meterSize.y *= rate;
		meterSprite->GetSprite()->SetAnchorPoint({ 0.0f, 1.0f });
		backgroundSprite->GetSprite()->SetAnchorPoint({ 0.0f, 1.0f });
		meterPos += Vector2(offsetSize.x * 0.5f, -offsetSize.x * 0.5f);
		break;
	case UIMeterType::Right: // 右方向
		meterSize.x *= rate;
		meterSprite->GetSprite()->SetAnchorPoint({ 1.0f, 0.5f });
		backgroundSprite->GetSprite()->SetAnchorPoint({ 0.0f, 0.5f });
		meterPos += Vector2(-(offsetSize.x * 0.5f) + backgroundSprite->GetHalfSise(SpriteSize::Right).x, 0);
		break;
	case UIMeterType::Left:	// 左方向
		meterSize.x *= rate;
		meterSprite->GetSprite()->SetAnchorPoint({ 0.0f, 0.5f });
		backgroundSprite->GetSprite()->SetAnchorPoint({ 0.0f, 0.5f });
		meterPos += Vector2(offsetSize.x * 0.5f, 0);
		break;
	case UIMeterType::WidthCenter:	// 横方向
		meterSize.x *= rate;
		meterSprite->GetSprite()->SetAnchorPoint({ 0.5f, 0.5f });
		backgroundSprite->GetSprite()->SetAnchorPoint({ 0.0f , 0.5f });
		meterPos += Vector2(-(offsetSize.x * 0.5f) * 0.5f + backgroundSprite->GetHalfSise(SpriteSize::Right).x * 0.5f, 0);
		break;
	case UIMeterType::HeightCenter:	// 縦方向
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
	
	// 更新
	meterSprite->Update();
	backgroundSprite->Update();
}

void Engine::UIMeter::UniqueDraw() {

	// 描画
	backgroundSprite->Draw();
	meterSprite->Draw();
	if (nameSprite_) {
		nameSprite_->Draw();
	}
}

#pragma endregion

#pragma region UIPair

void Engine::UIPair::InitSprite()
{
	// 最初のスプライト初期化
	firstSprite = std::make_unique<BaseSprite>();
	firstSprite->Init(entityManager, "first", "resources/Texture/Image.dds");
	// 次のスプライト初期化
	secondSprite = std::make_unique<BaseSprite>();
	secondSprite->Init(entityManager, "second", "resources/Texture/Image.dds");
	
	// 親子付け
	if (parent) {
		firstSprite->GetSprite()->GetWorldTransform2d().parent_ = parent;
		secondSprite->GetSprite()->GetWorldTransform2d().parent_ = parent;
	}
	
}

void Engine::UIPair::Update(float deltaTime) {
	firstSprite->GetSprite()->SetPosition(pos_);

	Vector2 secondpos = pos_;
	switch (type_)
	{
	case UIPairDrectionType::Top:	// 上方向
		secondpos.y -= offset_ + firstSprite->GetHalfSise(SpriteSize::Top).y;
		break;
	case UIPairDrectionType::Down:	// 下方向
		secondpos.y += offset_ + firstSprite->GetHalfSise(SpriteSize::Down).y;
		break;
	case UIPairDrectionType::Right:	// 右方向
		secondpos.x += offset_ + firstSprite->GetHalfSise(SpriteSize::Right).x;
		break;
	case UIPairDrectionType::Left:	// 左方向
		secondpos.x -= offset_ + firstSprite->GetHalfSise(SpriteSize::Left).x;
		break;
	default:
		break;
	}
	
	secondSprite->GetSprite()->SetPosition(secondpos);

	// 更新
	firstSprite->Update();
	secondSprite->Update();
}

void Engine::UIPair::UniqueDraw() {
	/// 描画
	firstSprite->Draw();
	secondSprite->Draw();
}

#pragma endregion

#pragma region UICount

void Engine::UICount::InitSprite()
{
	// 行数分初期化
	for (int i = 0; i < instance_; i++) {
		auto sprite = std::make_unique<BaseSprite>();
		sprite->Init(entityManager, "count", "resources/Texture/num/Number_x64y96.dds");
		sprite->SetSize({ 24.0f,24.0f });		// サイズ設定
		sprite->SetAnchorPoint({ 0.5f,0.5f });	// アンカーポイント設定

		countSprite_.push_back(std::move(sprite));
	}	
	// 名前スプライトがあるなら初期化
	if (useNameSprite_) {
		nameSprite_ = std::make_unique<BaseSprite>();
		nameSprite_->Init(entityManager, "name", "resources/Texture/Image.dds");
		nameSprite_->SetAnchorPoint(Vector2(0.0f, 0.5f));	// アンカーポイント設定
		nameSprite_->SetSize({ 24.0f,24.0f });				// サイズ設定
	}

	// 親子付け
	if (parent) {
		for (auto& sprite : countSprite_) {
			sprite->GetSprite()->GetWorldTransform2d().parent_ = parent;
		}
		if (useNameSprite_) {
			nameSprite_->GetSprite()->GetWorldTransform2d().parent_ = parent;
		}
	}
}


void Engine::UICount::Update(float deltaTime)
{
	if (count_ >= countMax_) {
		count_ = countMax_;
	}

	int count = (std::max)(0, static_cast<int>(count_)); // 念のためマイナス防止
	int numDigits = (count == 0) ? 1 : static_cast<int>(log10(static_cast<double>(count))) + 1;

	int i = 0;
	for (auto& sprite : countSprite_) {
		if (i >= numDigits) {
			break;
		} // 桁数を超えたら描画しない

		int digit = (count / static_cast<int>(pow(10, i))) % 10;

		sprite->GetSprite()->SetTextureLeftTop(Vector2((texSize_.x * digit), 0));
		Vector2 pos = pos_ + Vector2((texSize_.x * static_cast<float>((instance_ - 1) - i) * 0.5f), 0.0f) + (offsetSize * static_cast<float>((instance_ - 1) - i));
		sprite->GetSprite()->SetPosition(pos);
		sprite->Update();
		i++;
	}

	// 名前スプライトがあるなら
	if (nameSprite_) {
		nameSprite_->GetSprite()->SetPosition(countSprite_.front()->GetSprite()->GetPosition() + Vector2(offsetSize.x, 0));
		nameSprite_->Update();
	}
}


void Engine::UICount::UniqueDraw() {
	const int count = (std::max)(0, static_cast<int>(count_)); // 念のためマイナス防止
	const int numDigits = (count == 0) ? 1 : static_cast<int>(log10(static_cast<double>(count))) + 1;

	int i = 0;
	for (const auto& sprite : countSprite_) {
		if (i >= numDigits) {
			break; // 桁数を超えたら描画しない
		}
		sprite->Draw();
		i++;
	}

	if (nameSprite_) {
		nameSprite_->Draw();
	}
}

#pragma endregion
