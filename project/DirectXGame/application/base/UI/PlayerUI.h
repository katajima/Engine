#pragma once
#include "DirectXGame/application/base/UI/Base/BaseUI.h"

// 前方宣言
namespace Character {
	class ParameterComponent;
}
/// <summary>
/// プレイヤーUIクラス
/// </summary>
class PlayerUI : public BaseUI{
public:
	// 初期化
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables) override;

	// 更新
	void Update(float dt) override;

	// 描画
	void Draw() override;

private:

	// 操作UI
	void InitializeOperationUI();

	// プレイヤーUI
	void InitializePlayerUI();

	void UpdatePlayerUI(float dt);

public:

	void SetCharacterParameter(Character::ParameterComponent* parameter) { parameterComponent = parameter; };

	// スペシャルゲージサイズ設定
	void SetSpecialGaugeSize(float size) { sizeSpecialGauge_ = size; }
	// maxテクスチャ映すか
	void SetIsTextmax(bool is) { isTextMax_ = is; }
	// RBボタンテクスチャ映すか
	void SetIsTextRB(bool is) { isTextRB_ = is; }
private:
	Character::ParameterComponent* parameterComponent = nullptr;
	
	std::unique_ptr<Engine::Sprite> textMax_;
	std::unique_ptr<Engine::Sprite> textRB_;
	bool isTextRB_ = false;
	bool isTextMax_ = false;
	float sizeSpecialGauge_ = 0;
private:
	// メータUIデータ
	struct MeterUIData {
		Vector2 pos = { 30,600 };
		Color color = { 0,1,0,1 };
		Color nameColor = { 0.75f,0.75f ,0.75f ,1.0f };

		float maxMeter = 100.0f;
		Vector2 offset = { 4.0f,4.0f };

		Vector2 size = { 200.0f,30.0f };
		Vector2 nameSize = { 60.0f,40.0f };
	};
	MeterUIData hpSpriteData;
	MeterUIData spSpriteData;
	MeterUIData staminaSpriteData;

	struct TextData {
		Vector2 pos = { 1120,520 };
		Vector2 pos2 = { 1120,470 };

		Vector2 anchorPoint = { 0.5f,0.5f };
		Vector2 anchorPoint2 = { 0.0f,0.5f };

		Vector2 size = { 64.0f,64.0f };
		Vector2 size2 = { 48.0f,48.0f };

		float offset = 20.0f;

		Color color = { 0.75f,0.75f ,0.75f ,1.0f };
	};
	TextData attackTextData;
	TextData spTextData;
	TextData jumpTextData;
	TextData skillTextData;

	struct MaxTextData {
		float size_ = 0.25f;
		Vector2 pos_ = { 30,650 };
		float rotate_ = Math::DegreesToRadians(-30);
		Vector2 anchorPoint_ = { 0.5f,0.5f };
		Color color_ = { 1,0,0,1 };
	};
	MaxTextData maxTextData_;

	struct RBData {
		float size_ = 0.2f;
		Vector2 pos_ = { 1280 / 2,550 };
		float rotate_ = Math::DegreesToRadians(-30);
		Vector2 anchorPoint_ = { 0.5f,0.5f };
		Color color_ = { 1,1,1,1 };
	};
	RBData rbData_;

};

