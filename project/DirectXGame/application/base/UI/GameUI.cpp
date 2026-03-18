#include "GameUI.h"
#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayer.h"

#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

#include <DirectXGame/engine/Utility/ConvertUtility.h>

void GameUI::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables)
{
	this->entityManager = entityManager;
	this->globalVariables = globalVariables;
	this->inputSystem = inputSystem;

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

void GameUI::Update(float dt){
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