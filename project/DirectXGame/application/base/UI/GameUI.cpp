#include "GameUI.h"
#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayer.h"

#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

#include <DirectXGame/engine/Utility/ConvertUtility.h>

void GameUI::Initialize(Engine::Input* input, Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables)
{
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
	input_ = input;


	board_ = std::make_unique<Engine::UIBaseBoard>();
	board_->Init(input_,entity2DManager_, "bord", boardPos, boardSize);

	// 通常攻撃UI初期化
	InitUIPair("normalAttack", attackTextData_.pos_);
	Engine::UIPair* normalAttackPair = GetUIPair("normalAttack");
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
	Engine::UIPair* specialPair = GetUIPair("special");
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


	
	// 必殺技UI初期化
	InitUIPair("jump", jumpTextData.pos_);
	Engine::UIPair* jumpPair = GetUIPair("jump");
	jumpPair->SetOffset(jumpTextData.offset_);
	jumpPair->SetUIPairDrectionType(UIPairDrectionType::Right);			// 右方向
	jumpPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_button_color_a.png");
	jumpPair->GetFirstSprite()->SetAnchorPoint(jumpTextData.anchorPoint_);			// アンカーポイント設定
	jumpPair->GetFirstSprite()->SetSize(jumpTextData.size_);				// サイズ設定
	jumpPair->GetFirstSprite()->SetColor(jumpTextData.color_);	// 色設定

	jumpPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Jump.png");
	jumpPair->GetSecondSprite()->SetAnchorPoint(jumpTextData.anchorPoint2_);			// アンカーポイント設定
	jumpPair->GetSecondSprite()->SetSize(jumpTextData.size2_);						// サイズ設定
	jumpPair->GetSecondSprite()->SetColor(jumpTextData.color_);	// 色設定


	// ダッシュUI初期化
	InitUIPair("dush", dashTextData.pos_);
	Engine::UIPair* dushPair = GetUIPair("dush");
	dushPair->SetOffset(dashTextData.offset_);
	dushPair->SetUIPairDrectionType(UIPairDrectionType::Right);			// 右方向
	dushPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_lt.png");
	dushPair->GetFirstSprite()->SetAnchorPoint(dashTextData.anchorPoint_);			// アンカーポイント設定
	dushPair->GetFirstSprite()->SetSize(dashTextData.size_);				// サイズ設定
	dushPair->GetFirstSprite()->SetColor(dashTextData.color_);	// 色設定

	dushPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Dush.png");
	dushPair->GetSecondSprite()->SetAnchorPoint(dashTextData.anchorPoint2_);			// アンカーポイント設定
	dushPair->GetSecondSprite()->SetSize(dashTextData.size2_);						// サイズ設定
	dushPair->GetSecondSprite()->SetColor(dashTextData.color_);	// 色設定





	// クリアUI
	text_clera = std::make_unique<Engine::Sprite>();
	InitSprite(text_clera.get(), "resources/Texture/text/clear.png", cleraTextData_.pos_, cleraTextData_.scale_);
	text_clera->SetSize(cleraTextData_.size_);
	text_clera->SetColor(cleraTextData_.color_);

	// ゲームオーバーUI
	text_over = std::make_unique<Engine::Sprite>();
	InitSprite(text_over.get(), "resources/Texture/text/over.png", cleraTextData_.pos_, cleraTextData_.scale_);
	text_over->SetSize(cleraTextData_.size_);
	text_over->SetColor(cleraTextData_.color_);

	// カウントUI
	InitUICount("hitCount", hitTextData.pos_, hitTextData.instance,true);
	Engine::UICount* hitCount = GetUICount("hitCount");
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
	Engine::UISlider* slider = board_->GetUIElement<Engine::UISlider>(UIType::UISlider, "slider");
	slider->SetPos(sliderPos);

	// ボード更新
	board_->Update(0);
	
	

	Engine::UICount* hitCount = GetUICount("hitCount");
	hitCount->SetCount(ConvertUtility::ToFloat(player_->GetAttackController()->GetHitCounter().GetHitCount()));
	
	
	if (player_->GetAttackController()->GetHitCounter().GetHitCount() <= 0) {
		
		hitCount->SetCountColor({ 1,1,1,0.0f });
		hitCount->GetNameSprite()->SetColor({ 1,1,1,0.0f });
	}
	else {
		float t = player_->GetAttackController()->GetHitCounter().GetHitData().hitTime.LerpT();
		bool isT = t >= 0.5f;

		Color coNn = hitTextData.color_;
		Color coNe = hitTextData.nameColor_;
		
		
		if (isT) {
			hitCount->SetCountColor({ coNn.r ,coNn.g ,coNn.b ,1.0f - t });
			hitCount->GetNameSprite()->SetColor({ coNe.r ,coNe.g ,coNe.b ,1.0f - t });
		}
		else {
			hitCount->SetCountColor(coNn);
			hitCount->GetNameSprite()->SetColor(coNe);
		}
	}

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