#include "GameUI.h"
#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayer.h"

#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void GameUI::Initialize(Input* input,Entity2DManager* entity2DManager, GlobalVariables* globalVariables)
{
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
	input_ = input;
	float xpos = 1050;
	Vector2 scale{ 75,75 };
	

	board_ = std::make_unique<UIBaseBoard>();
	board_->Init(input_,entity2DManager_, "bord", { 100,100 }, { 400,400 });

	// 通常攻撃UI初期化
	InitUIPair("normalAttack", { 1120,520 });
	UIPair* normalAttackPair = GetUIPair("normalAttack");
	normalAttackPair->SetOffset(20.0f);	// 間隔設定
	normalAttackPair->SetUIPairDrectionType(UIPairDrectionType::Right);	// 右方向
	normalAttackPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_button_color_b.png"); // 
	normalAttackPair->GetFirstSprite()->SetAnchorPoint({0.5f,0.5f});	// アンカーポイント設定
	normalAttackPair->GetFirstSprite()->SetSize({64.0f,64.0f });		// サイズ設定
	normalAttackPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Attack.png");
	normalAttackPair->GetSecondSprite()->SetAnchorPoint({ 0.0f,0.5f }); // アンカーポイント設定
	normalAttackPair->GetSecondSprite()->SetSize({ 32,32 });			// サイズ設定
	InitUIPair("special", { 1120,570 });

	// 必殺技UI初期化
	UIPair* specialPair = GetUIPair("special");
	specialPair->SetOffset(20.0f);
	specialPair->SetUIPairDrectionType(UIPairDrectionType::Right);			// 右方向
	specialPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_rb.png");
	specialPair->GetFirstSprite()->SetAnchorPoint({ 0.5f,0.5f });			// アンカーポイント設定
	specialPair->GetFirstSprite()->SetSize({ 64.0f,64.0f });				// サイズ設定
	specialPair->GetFirstSprite()->SetColor({ 0.75f,0.75f ,0.75f ,1.0f });	// 色設定

	specialPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Special.png");
	specialPair->GetSecondSprite()->SetAnchorPoint({ 0.0f,0.5f });			// アンカーポイント設定
	specialPair->GetSecondSprite()->SetSize({ 32,32 });						// サイズ設定
	specialPair->GetSecondSprite()->SetColor({ 0.75f,0.75f ,0.75f ,1.0f });	// 色設定

	xpos = 1120;
	scale = { 100 * 1.5f,33 * 1.5f };

	// クリアUI
	text_clera = std::make_unique<Sprite>();
	InitSprite(text_clera.get(), "resources/Texture/text/clear.png", { 300,200 }, scale);
	text_clera->SetSize(2);
	text_clera->SetColor({ 0,1,0,1 });

	// ゲームオーバーUI
	text_over = std::make_unique<Sprite>();
	InitSprite(text_over.get(), "resources/Texture/text/over.png", { 300,200 }, scale);
	text_over->SetSize(2);
	text_over->SetColor({ 1,0,0,1 });

	// カウントUI
	InitUICount("hitCount", { 950,100 }, 3,true);
	UICount* hitCount = GetUICount("hitCount");
	hitCount->SetMaxSize({ 64 * 2 / 3, 96 * 2 / 3 }, { 20.0f,0.0f });	// 最大サイズ
	hitCount->SetTextuerSize({ 64,96 });				// テクスチャサイズ
	hitCount->SetCountMax(999);							// カウント最大数
	hitCount->SetCountColor({ 0.9f,0.9f ,0.9f ,1.0f });	// 色指定
	hitCount->GetNameSprite()->SetTextureName("resources/Texture/text/Hit.png");	
	hitCount->GetNameSprite()->SetSize({ 100 * 1.5f,33 * 1.5f });	// サイズ指定
	hitCount->GetNameSprite()->SetColor({0.9f,0.9f ,0.9f ,1.0f});	// 色指定


	// UIボード
	board_->CreateUIElement(UIType::CheckBox, "botton",{100,100});
	board_->CreateUIElement(UIType::UISlider, "slider",{100,100});

}

void GameUI::Update()
{
	// ボード画面比率画面左上座標指定
	board_->SetImageLeftTopPosAndRatio(leftTopPos_,ratio_);

	// スライダー位置
	UISlider* slider = board_->GetUIElement<UISlider>(UIType::UISlider, "slider");
	slider->SetPos({30,50});

	// ボード更新
	board_->Update(0);
	
	

	UICount* hitCount = GetUICount("hitCount");
	
	// 更新
	UpdateUIElement(0);
}

void GameUI::Draw()
{
	// 描画
	DrawUIElement();
}

void GameUI::SetPlayer(BasePlayer* player)
{
	player_ = player;
}