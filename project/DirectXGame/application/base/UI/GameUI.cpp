#include "GameUI.h"
#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayer.h"

#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

#include <DirectXGame/engine/Utility/ConvertUtility.h>

void GameUI::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables)
{
	this->entityManager = entityManager;
	this->globalVariables = globalVariables;
	this->inputSystem = inputSystem;


	board_ = std::make_unique<Engine::UIBaseBoard>();
	board_->Init(inputSystem,entityManager, "bord", boardPos, boardSize);

	// 通常攻撃UI初期化
	InitUIPair("normalAttack", attackTextData_.pos_);
	Engine::UIPair* normalAttackPair = GetUIPair("normalAttack");
	normalAttackPair->SetOffset(attackTextData_.offset_);	// 間隔設定
	normalAttackPair->SetUIPairDrectionType(UIPairDrectionType::Right);	// 右方向
	normalAttackPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_button_color_b.dds"); // 
	normalAttackPair->GetFirstSprite()->SetAnchorPoint(attackTextData_.anchorPoint_);	// アンカーポイント設定
	normalAttackPair->GetFirstSprite()->SetSize(attackTextData_.size_);		// サイズ設定
	normalAttackPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Attack.dds");
	normalAttackPair->GetSecondSprite()->SetAnchorPoint(attackTextData_.anchorPoint2_); // アンカーポイント設定
	normalAttackPair->GetSecondSprite()->SetSize(attackTextData_.size2_);			// サイズ設定
	
	// 通常攻撃UI初期化
	InitUIPair("havyAttack", attackTextData_.pos2_);
	Engine::UIPair* havyAttackPair = GetUIPair("havyAttack");
	havyAttackPair->SetOffset(attackTextData_.offset_);	// 間隔設定
	havyAttackPair->SetUIPairDrectionType(UIPairDrectionType::Right);	// 右方向
	havyAttackPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_button_color_x.dds"); // 
	havyAttackPair->GetFirstSprite()->SetAnchorPoint(attackTextData_.anchorPoint_);	// アンカーポイント設定
	havyAttackPair->GetFirstSprite()->SetSize(attackTextData_.size_);		// サイズ設定
	havyAttackPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Attack.dds");
	havyAttackPair->GetSecondSprite()->SetAnchorPoint(attackTextData_.anchorPoint2_); // アンカーポイント設定
	havyAttackPair->GetSecondSprite()->SetSize(attackTextData_.size2_);			// サイズ設定



	InitUIPair("special", spTextData.pos_);

	// 必殺技UI初期化
	Engine::UIPair* specialPair = GetUIPair("special");
	specialPair->SetOffset(spTextData.offset_);
	specialPair->SetUIPairDrectionType(UIPairDrectionType::Right);			// 右方向
	specialPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_rb.dds");
	specialPair->GetFirstSprite()->SetAnchorPoint(spTextData.anchorPoint_);			// アンカーポイント設定
	specialPair->GetFirstSprite()->SetSize(spTextData.size_);				// サイズ設定
	specialPair->GetFirstSprite()->SetColor(spTextData.color_);	// 色設定

	specialPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Special.dds");
	specialPair->GetSecondSprite()->SetAnchorPoint(spTextData.anchorPoint2_);			// アンカーポイント設定
	specialPair->GetSecondSprite()->SetSize(spTextData.size2_);						// サイズ設定
	specialPair->GetSecondSprite()->SetColor(spTextData.color_);	// 色設定


	
	// 必殺技UI初期化
	InitUIPair("jump", jumpTextData.pos_);
	Engine::UIPair* jumpPair = GetUIPair("jump");
	jumpPair->SetOffset(jumpTextData.offset_);
	jumpPair->SetUIPairDrectionType(UIPairDrectionType::Right);			// 右方向
	jumpPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_button_color_a.dds");
	jumpPair->GetFirstSprite()->SetAnchorPoint(jumpTextData.anchorPoint_);			// アンカーポイント設定
	jumpPair->GetFirstSprite()->SetSize(jumpTextData.size_);				// サイズ設定
	jumpPair->GetFirstSprite()->SetColor(jumpTextData.color_);	// 色設定

	jumpPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Jump.dds");
	jumpPair->GetSecondSprite()->SetAnchorPoint(jumpTextData.anchorPoint2_);			// アンカーポイント設定
	jumpPair->GetSecondSprite()->SetSize(jumpTextData.size2_);						// サイズ設定
	jumpPair->GetSecondSprite()->SetColor(jumpTextData.color_);	// 色設定


	//// ダッシュUI初期化
	//InitUIPair("dush", dashTextData.pos_);
	//Engine::UIPair* dushPair = GetUIPair("dush");
	//dushPair->SetOffset(dashTextData.offset_);
	//dushPair->SetUIPairDrectionType(UIPairDrectionType::Right);			// 右方向
	//dushPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_lt.dds");
	//dushPair->GetFirstSprite()->SetAnchorPoint(dashTextData.anchorPoint_);			// アンカーポイント設定
	//dushPair->GetFirstSprite()->SetSize(dashTextData.size_);				// サイズ設定
	//dushPair->GetFirstSprite()->SetColor(dashTextData.color_);	// 色設定

	//dushPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Dush.dds");
	//dushPair->GetSecondSprite()->SetAnchorPoint(dashTextData.anchorPoint2_);			// アンカーポイント設定
	//dushPair->GetSecondSprite()->SetSize(dashTextData.size2_);						// サイズ設定
	//dushPair->GetSecondSprite()->SetColor(dashTextData.color_);	// 色設定


	// スキルUI初期化
	InitUIPair("skill", skillTextData.pos_);
	Engine::UIPair* skillPair = GetUIPair("skill");
	skillPair->SetOffset(skillTextData.offset_);
	skillPair->SetUIPairDrectionType(UIPairDrectionType::Right);			// 右方向
	skillPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_button_color_y.dds");
	skillPair->GetFirstSprite()->SetAnchorPoint(skillTextData.anchorPoint_);			// アンカーポイント設定
	skillPair->GetFirstSprite()->SetSize(skillTextData.size_);				// サイズ設定
	skillPair->GetFirstSprite()->SetColor(skillTextData.color_);	// 色設定

	skillPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Dush.dds");
	skillPair->GetSecondSprite()->SetAnchorPoint(skillTextData.anchorPoint2_);			// アンカーポイント設定
	skillPair->GetSecondSprite()->SetSize(skillTextData.size2_);						// サイズ設定
	skillPair->GetSecondSprite()->SetColor(skillTextData.color_);	// 色設定



	// クリアUI
	text_clera = std::make_unique<Engine::Sprite>();
	InitSprite(text_clera.get(), "resources/Texture/text/clear.dds", cleraTextData_.pos_, cleraTextData_.scale_);
	text_clera->SetSize(cleraTextData_.size_);
	text_clera->SetColor(cleraTextData_.color_);

	// ゲームオーバーUI
	text_over = std::make_unique<Engine::Sprite>();
	InitSprite(text_over.get(), "resources/Texture/text/over.dds", cleraTextData_.pos_, cleraTextData_.scale_);
	text_over->SetSize(cleraTextData_.size_);
	text_over->SetColor(cleraTextData_.color_);

	text_bar = std::make_unique<Engine::Sprite>();
	InitSprite(text_bar.get(), "resources/Texture/Image.dds", barTextData_.pos_, barTextData_.scale_);
	text_bar->SetSize(barTextData_.size_);
	text_bar->SetColor(barTextData_.color_);

	// カウントUI
	InitUICount("hitCount", hitTextData.pos_, hitTextData.instance,true);
	Engine::UICount* hitCount = GetUICount("hitCount");
	hitCount->SetMaxSize(hitTextData.size, hitTextData.offset);	// 最大サイズ
	hitCount->SetTextuerSize(hitTextData.textuerSize);				// テクスチャサイズ
	hitCount->SetCountMax(hitTextData.countmax);							// カウント最大数
	hitCount->SetCountColor(hitTextData.color_);	// 色指定
	hitCount->GetNameSprite()->SetTextureName("resources/Texture/text/Hit.dds");	
	hitCount->GetNameSprite()->SetSize(hitTextData.nameSize);	// サイズ指定
	hitCount->GetNameSprite()->SetColor(hitTextData.nameColor_);	// 色指定


	// UIボード
	board_->CreateUIElement(UIType::CheckBox, "botton", boardPos);
	board_->CreateUIElement(UIType::UISlider, "slider", boardPos);



	// カウントUI(ウェーブ)
	InitUICount("WaveCount", waveTextData.pos_, waveTextData.instance, true);
	Engine::UICount* waveCount = GetUICount("WaveCount");
	waveCount->SetMaxSize(waveTextData.size, waveTextData.offset);	// 最大サイズ
	waveCount->SetTextuerSize(waveTextData.textuerSize);				// テクスチャサイズ
	waveCount->SetCountMax(waveTextData.countmax);							// カウント最大数
	waveCount->SetCountColor(waveTextData.color_);	// 色指定
	waveCount->GetNameSprite()->SetTextureName("resources/Texture/text/wave.dds");
	waveCount->GetNameSprite()->SetSize(waveTextData.nameSize);	// サイズ指定
	waveCount->GetNameSprite()->SetColor(waveTextData.nameColor_);	// 色指定


	//// カウントUI(秒数)
	InitUICount("WaveSecondsCount", secondTextData.pos_, secondTextData.instance, true);
	Engine::UICount* waveSecondsCount = GetUICount("WaveSecondsCount");
	waveSecondsCount->SetMaxSize(secondTextData.size, secondTextData.offset);	// 最大サイズ
	waveSecondsCount->SetTextuerSize(secondTextData.textuerSize);				// テクスチャサイズ
	waveSecondsCount->SetCountMax(secondTextData.countmax);							// カウント最大数
	waveSecondsCount->SetCountColor(secondTextData.color_);	// 色指定
	waveSecondsCount->GetNameSprite()->SetTextureName("resources/Texture/text/seconds.dds");
	waveSecondsCount->GetNameSprite()->SetSize(secondTextData.nameSize);	// サイズ指定
	waveSecondsCount->GetNameSprite()->SetColor(secondTextData.nameColor_);	// 色指定

}

void GameUI::Update(float dt)
{
	// ボード画面比率画面左上座標指定
	board_->SetImageLeftTopPosAndRatio(leftTopPos_,ratio_);

	// スライダー位置
	Engine::UISlider* slider = board_->GetUIElement<Engine::UISlider>(UIType::UISlider, "slider");
	slider->SetPos(sliderPos);

	// ボード更新
	board_->Update(0);
	
	

	Engine::UICount* hitCount = GetUICount("hitCount");
	hitCount->SetCount(ConvertUtility::ToFloat(player->GetAttackController()->GetHitCounter().GetHitCount()));
	
	
	if (player->GetAttackController()->GetHitCounter().GetHitCount() <= 0) {
		
		hitCount->SetCountColor({ 1,1,1,0.0f });
		hitCount->GetNameSprite()->SetColor({ 1,1,1,0.0f });
	}
	else {
		float t = player->GetAttackController()->GetHitCounter().GetHitData().hitTime.LerpT();
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


	Engine::UICount* waveCount = GetUICount("WaveCount");
	waveCount->SetCount(ConvertUtility::ToFloat(data_.flowData_.currentWave));

	
	Engine::UICount* waveSecondsCount = GetUICount("WaveSecondsCount");
	waveSecondsCount->SetCount(data_.flowData_.elapsedTime);


	text_bar->Update();
	// 更新
	UpdateUIElement(0);
}

void GameUI::Draw()
{
	text_bar->Draw();

	// 描画
	DrawUIElement();
}

void GameUI::SetPlayer(Character::BasePlayer* player)
{
	this->player = player;
}