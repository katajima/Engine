#include "GameUI.h"
#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayer.h"

#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

#include <DirectXGame/engine/Utility/ConvertUtility.h>

void GameUI::Initialize(Input* input,Entity2DManager* entity2DManager, GlobalVariables* globalVariables)
{
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
	input_ = input;


	board_ = std::make_unique<UIBaseBoard>();
	board_->Init(input_,entity2DManager_, "bord", boardPos, boardSize);

	// 通常攻撃UI初期化
	InitUIPair("normalAttack", attackTextData_.pos_);
	UIPair* normalAttackPair = GetUIPair("normalAttack");
	normalAttackPair->SetOffset(attackTextData_.offset_);	// 間隔設定
	normalAttackPair->SetUIPairDrectionType(UIPairDrectionType::Right);	// 右方向
	normalAttackPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_button_color_b.png"); // 
	normalAttackPair->GetFirstSprite()->SetAnchorPoint(attackTextData_.anchorPoint_);	// アンカーポイント設定
	normalAttackPair->GetFirstSprite()->SetSize(attackTextData_.size_);		// サイズ設定
	normalAttackPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Attack.png");
	normalAttackPair->GetSecondSprite()->SetAnchorPoint(attackTextData_.anchorPoint2_); // アンカーポイント設定
	normalAttackPair->GetSecondSprite()->SetSize(attackTextData_.size2_);			// サイズ設定
	
	InitUIPair("special", spTextData.pos_);

	// 必殺技UI初期化
	UIPair* specialPair = GetUIPair("special");
	specialPair->SetOffset(spTextData.offset_);
	specialPair->SetUIPairDrectionType(UIPairDrectionType::Right);			// 右方向
	specialPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_rb.png");
	specialPair->GetFirstSprite()->SetAnchorPoint(spTextData.anchorPoint_);			// アンカーポイント設定
	specialPair->GetFirstSprite()->SetSize(spTextData.size_);				// サイズ設定
	specialPair->GetFirstSprite()->SetColor(spTextData.color_);	// 色設定

	specialPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Special.png");
	specialPair->GetSecondSprite()->SetAnchorPoint(spTextData.anchorPoint2_);			// アンカーポイント設定
	specialPair->GetSecondSprite()->SetSize(spTextData.size2_);						// サイズ設定
	specialPair->GetSecondSprite()->SetColor(spTextData.color_);	// 色設定


	// クリアUI
	text_clera = std::make_unique<Sprite>();
	InitSprite(text_clera.get(), "resources/Texture/text/clear.png", cleraTextData_.pos_, cleraTextData_.scale_);
	text_clera->SetSize(cleraTextData_.size_);
	text_clera->SetColor(cleraTextData_.color_);

	// ゲームオーバーUI
	text_over = std::make_unique<Sprite>();
	InitSprite(text_over.get(), "resources/Texture/text/over.png", cleraTextData_.pos_, cleraTextData_.scale_);
	text_over->SetSize(cleraTextData_.size_);
	text_over->SetColor(cleraTextData_.color_);

	// カウントUI
	InitUICount("hitCount", hitTextData.pos_, hitTextData.instance,true);
	UICount* hitCount = GetUICount("hitCount");
	hitCount->SetMaxSize(hitTextData.size, hitTextData.offset);	// 最大サイズ
	hitCount->SetTextuerSize(hitTextData.textuerSize);				// テクスチャサイズ
	hitCount->SetCountMax(hitTextData.countmax);							// カウント最大数
	hitCount->SetCountColor(hitTextData.color_);	// 色指定
	hitCount->GetNameSprite()->SetTextureName("resources/Texture/text/Hit.png");	
	hitCount->GetNameSprite()->SetSize(hitTextData.nameSize);	// サイズ指定
	hitCount->GetNameSprite()->SetColor(hitTextData.nameColor_);	// 色指定


	// UIボード
	board_->CreateUIElement(UIType::CheckBox, "botton", boardPos);
	board_->CreateUIElement(UIType::UISlider, "slider", boardPos);

}

void GameUI::Update()
{
	// ボード画面比率画面左上座標指定
	board_->SetImageLeftTopPosAndRatio(leftTopPos_,ratio_);

	// スライダー位置
	UISlider* slider = board_->GetUIElement<UISlider>(UIType::UISlider, "slider");
	slider->SetPos(sliderPos);

	// ボード更新
	board_->Update(0);
	
	

	UICount* hitCount = GetUICount("hitCount");
	hitCount->SetCount(ConvertUtility::ToFloat(player_->GetAttackController()->GetHitCounter().GetHitCount()));
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