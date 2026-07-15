#include "PlayerUI.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"
#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include <cmath>

void PlayerUI::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables)
{
	this->entityManager = entityManager;	// エンティティ
	this->globalVariables = globalVariables;	// 保存項目
	this->inputSystem = inputSystem;	// インプット

	// プレイヤーUI初期化
	InitializePlayerUI();
	// 操作UI初期化
	InitializeOperationUI();
}

void PlayerUI::Update(float dt) {
	// プレイヤーUI更新
	UpdatePlayerUI(dt);
	// 更新
	UpdateUIElement(dt);
}

void PlayerUI::Draw()
{
	// 描画
	DrawUIElement();
}

void PlayerUI::SetFollowTarget(Engine::WorldTransform* target, Engine::Camera* camera) {
	followTarget_ = target;		// UIを追従させるプレイヤー本体
	followCamera_ = camera;		// ワールド座標から画面座標へ変換するカメラ
}

void PlayerUI::InitializeOperationUI() {
	// 通常攻撃UI初期化
	InitUIPair("normalAttack", attackTextData.pos);
	Engine::UIPair* normalAttackPair = GetUIPair("normalAttack");
	normalAttackPair->SetOffset(attackTextData.offset);	// 間隔設定
	normalAttackPair->SetUIPairDirectionType(UIPairDirectionType::Right);	// 右方向
	normalAttackPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_button_color_x.dds"); //
	normalAttackPair->GetFirstSprite()->SetAnchorPoint(attackTextData.anchorPoint);	// アンカーポイント設定
	normalAttackPair->GetFirstSprite()->SetSize(attackTextData.size);		// サイズ設定
	normalAttackPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Attack.dds");
	normalAttackPair->GetSecondSprite()->SetAnchorPoint(attackTextData.anchorPoint2); // アンカーポイント設定
	normalAttackPair->GetSecondSprite()->SetSize(attackTextData.size2);			// サイズ設定

	// 通常攻撃UI初期化
	InitUIPair("heavyAttack", attackTextData.pos2);
	Engine::UIPair* heavyAttackPair = GetUIPair("heavyAttack");
	heavyAttackPair->SetOffset(attackTextData.offset);	// 間隔設定
	heavyAttackPair->SetUIPairDirectionType(UIPairDirectionType::Right);	// 右方向
	heavyAttackPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_button_color_y.dds"); //
	heavyAttackPair->GetFirstSprite()->SetAnchorPoint(attackTextData.anchorPoint);	// アンカーポイント設定
	heavyAttackPair->GetFirstSprite()->SetSize(attackTextData.size);		// サイズ設定
	heavyAttackPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Attack.dds");
	heavyAttackPair->GetSecondSprite()->SetAnchorPoint(attackTextData.anchorPoint2); // アンカーポイント設定
	heavyAttackPair->GetSecondSprite()->SetSize(attackTextData.size2);			// サイズ設定

	// 必殺技UI初期化
	InitUIPair("special", spTextData.pos + Vector2{0,50});
	Engine::UIPair* specialPair = GetUIPair("special");
	specialPair->SetOffset(spTextData.offset);
	specialPair->SetUIPairDirectionType(UIPairDirectionType::Right);			// 右方向
	specialPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_rb.dds");
	specialPair->GetFirstSprite()->SetAnchorPoint(spTextData.anchorPoint);			// アンカーポイント設定
	specialPair->GetFirstSprite()->SetSize(spTextData.size);				// サイズ設定
	specialPair->GetFirstSprite()->SetColor(spTextData.color);	// 色設定
	specialPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Special.dds");
	specialPair->GetSecondSprite()->SetAnchorPoint(spTextData.anchorPoint2);			// アンカーポイント設定
	specialPair->GetSecondSprite()->SetSize(spTextData.size2);						// サイズ設定
	specialPair->GetSecondSprite()->SetColor(spTextData.color);	// 色設定



	// ジャンプUI初期化
	InitUIPair("jump", jumpTextData.pos + Vector2{ 0,100 });
	Engine::UIPair* jumpPair = GetUIPair("jump");
	jumpPair->SetOffset(jumpTextData.offset);
	jumpPair->SetUIPairDirectionType(UIPairDirectionType::Right);			// 右方向
	jumpPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_button_color_a.dds");
	jumpPair->GetFirstSprite()->SetAnchorPoint(jumpTextData.anchorPoint);			// アンカーポイント設定
	jumpPair->GetFirstSprite()->SetSize(jumpTextData.size);				// サイズ設定
	jumpPair->GetFirstSprite()->SetColor(jumpTextData.color);	// 色設定
	jumpPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Jump.dds");
	jumpPair->GetSecondSprite()->SetAnchorPoint(jumpTextData.anchorPoint2);			// アンカーポイント設定
	jumpPair->GetSecondSprite()->SetSize(jumpTextData.size2);						// サイズ設定
	jumpPair->GetSecondSprite()->SetColor(jumpTextData.color);	// 色設定


	// スキルUI初期化
	InitUIPair("skill", skillTextData.pos + Vector2{ 0,150 });
	Engine::UIPair* skillPair = GetUIPair("skill");
	skillPair->SetOffset(skillTextData.offset);
	skillPair->SetUIPairDirectionType(UIPairDirectionType::Right);			// 右方向
	skillPair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_button_color_b.dds");
	skillPair->GetFirstSprite()->SetAnchorPoint(skillTextData.anchorPoint);			// アンカーポイント設定
	skillPair->GetFirstSprite()->SetSize(skillTextData.size);				// サイズ設定
	skillPair->GetFirstSprite()->SetColor(skillTextData.color);	// 色設定
	skillPair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Attack.dds");
	skillPair->GetSecondSprite()->SetAnchorPoint(skillTextData.anchorPoint2);			// アンカーポイント設定
	skillPair->GetSecondSprite()->SetSize(skillTextData.size2);						// サイズ設定
	skillPair->GetSecondSprite()->SetColor(skillTextData.color);	// 色設定

	// 回避UI初期化
	InitUIPair("evade", evadeTextData.pos + Vector2{ 0,200 });
	Engine::UIPair* evadePair = GetUIPair("evade");
	evadePair->SetOffset(evadeTextData.offset);
	evadePair->SetUIPairDirectionType(UIPairDirectionType::Right);			// 右方向
	evadePair->GetFirstSprite()->SetTextureName("resources/Texture/XBOX/xbox_lb.dds");
	evadePair->GetFirstSprite()->SetAnchorPoint(evadeTextData.anchorPoint);			// アンカーポイント設定
	evadePair->GetFirstSprite()->SetSize(evadeTextData.size);				// サイズ設定
	evadePair->GetFirstSprite()->SetColor(evadeTextData.color);	// 色設定
	evadePair->GetSecondSprite()->SetTextureName("resources/Texture/icon/Dash.dds");
	evadePair->GetSecondSprite()->SetAnchorPoint(evadeTextData.anchorPoint2);			// アンカーポイント設定
	evadePair->GetSecondSprite()->SetSize(evadeTextData.size2);						// サイズ設定
	evadePair->GetSecondSprite()->SetColor(evadeTextData.color);	// 色設定


}

void PlayerUI::InitializePlayerUI() {

	// HPUI初期化
	InitUIMeter("HPBer", hpSpriteData.pos, false);		// 初期化HP
	Engine::UIMeter* hpber = GetUIMeter("HPBer");
	hpber->SetMaxSize(hpSpriteData.size, hpSpriteData.offset);				// 最大サイズ
	hpber->SetMeterMinMax(0.0f, hpSpriteData.maxMeter);						// メータ最大値最小値
	hpber->GetMeterSprite()->SetColor(hpSpriteData.color);					// 色指定
	hpber->SetMeterType(UIMeterType::Left);										// メータの増える方向

	// 初期化スペシャルUI
	spSpriteData.color = { 0,0,1,0.75f };
	spSpriteData.maxMeter = 20.0f;
	InitUIMeter("SpecialBar", spSpriteData.pos + Vector2{ 0,50 }, false);
	Engine::UIMeter* specialBar = GetUIMeter("SpecialBar");
	specialBar->SetMaxSize(spSpriteData.size, spSpriteData.offset);		// 最大サイズ
	specialBar->SetMeterMinMax(0.0f, spSpriteData.maxMeter);					// メータ最大値最小値
	specialBar->GetMeterSprite()->SetColor(spSpriteData.color);				// 色指定
	specialBar->SetMeterType(UIMeterType::Left);									// メータの増える方向

	// スタミナUI初期化
	staminaSpriteData.maxMeter = 100;
	staminaSpriteData.color = { 1,1,0,0.75f };
	staminaSpriteData.nameSize.x = 256;
	InitUIMeter("StaminaBer", staminaSpriteData.pos + Vector2{ 0,100 }, false);
	Engine::UIMeter* staminaBer = GetUIMeter("StaminaBer");
	staminaBer->SetMaxSize(staminaSpriteData.size, staminaSpriteData.offset);		// 最大サイズ
	staminaBer->SetMeterMinMax(0.0f, staminaSpriteData.maxMeter);					// メータ最大値最小値
	staminaBer->GetMeterSprite()->SetColor(staminaSpriteData.color);				// 色指定
	staminaBer->SetMeterType(UIMeterType::Left);									// メータの増える方向
}

void PlayerUI::UpdatePlayerUI(float dt) {
	Engine::UIMeter* hpber = GetUIMeter("HPBer");			// HP
	Engine::UIMeter* specialBar = GetUIMeter("SpecialBar");	// スペシャル
	Engine::UIMeter* staminaBer = GetUIMeter("StaminaBer");	// スタミナ
	if (!hpber || !specialBar || !staminaBer || !parameterComponent) {
		return;
	}

	// 追従対象がある場合は、プレイヤー頭上のワールド座標をスクリーン座標へ変換する。
	Vector2 basePos = hpSpriteData.pos;
	if (followTarget_ && followCamera_) {
		Engine::WorldTransform followPoint;
		followPoint.Initialize();
		followPoint.translate_ = followTarget_->GetWorldPosition() + followUIData_.worldOffset;
		followPoint.Update();

		basePos = GetScreenPos(followPoint, followCamera_);
		if (basePos.x <= -99.0f && basePos.y <= -99.0f) {
			basePos = followUIData_.hiddenPos;
		}
		else {
			basePos += followUIData_.screenOffset;
		}
	}

	// HP/SP/Staminaをプレイヤー上へ縦並びで配置する。
	hpber->SetPos(basePos);
	specialBar->SetPos(basePos + Vector2{ 0.0f,followUIData_.rowInterval });
	staminaBer->SetPos(basePos + Vector2{ 0.0f,followUIData_.rowInterval * 2.0f });
	
	hpber->SetMeterMinMax(parameterComponent->HP().minValue, parameterComponent->HP().maxValue);	// メータ最大値
	hpber->SetMeter(parameterComponent->HP().value);								// メータ

	staminaBer->SetMeterMinMax(parameterComponent->Stamina().minValue, parameterComponent->Stamina().maxValue);
	staminaBer->SetMeter(parameterComponent->Stamina().value);

	specialBar->SetMeter(sizeSpecialGauge_);// メータ
	UpdateSpecialReadyVisual(dt, specialBar);
}

void PlayerUI::UpdateSpecialReadyVisual(float dt, Engine::UIMeter* specialBar) {
	// SPが満タンの間だけ、ゲージと案内アイコンを明るく点滅させる。
	if (isTextMax_) {
		specialReadyEffectTimer_ += dt;
		const float pulse = (std::sin(specialReadyEffectTimer_ * 8.0f) + 1.0f) * 0.5f;
		const Color readyGaugeColor = {
			0.2f + 0.6f * pulse,
			0.8f + 0.2f * pulse,
			1.0f,
			0.75f
		};
		const Color readyIconColor = {
			1.0f,
			0.85f + 0.15f * pulse,
			0.2f + 0.8f * pulse,
			0.75f
		};

		specialBar->GetMeterSprite()->SetColor(readyGaugeColor);
		
		if (Engine::UIPair* specialPair = GetUIPair("special")) {
			specialPair->GetFirstSprite()->SetColor(readyIconColor);
			specialPair->GetSecondSprite()->SetColor(readyIconColor);
		}
		return;
	}

	// 満タンでない時は通常色へ戻し、次回満タン時の点滅を最初から始める。
	specialReadyEffectTimer_ = 0.0f;
	specialBar->GetMeterSprite()->SetColor(spSpriteData.color);
	
	if (Engine::UIPair* specialPair = GetUIPair("special")) {
		specialPair->GetFirstSprite()->SetColor(spTextData.color);
		specialPair->GetSecondSprite()->SetColor(spTextData.color);
	}
}
