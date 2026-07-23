#pragma once
#include "DirectXGame/application/base/UI/Base/BaseUI.h"

// 前方宣言
namespace Character {
	class ParameterComponent;
}
namespace Engine {
	class Camera;
	class WorldTransform;
}
/// <summary>
/// プレイヤーUIクラス
/// </summary>
class PlayerUI : public BaseUI{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt) override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

private:

	/// <summary>
	/// 操作UI
	/// </summary>
	void InitializeOperationUI();

	/// <summary>
	/// プレイヤーUI
	/// </summary>
	void InitializePlayerUI();

	/// <summary>
	/// SP満タン時の見た目更新
	/// </summary>
	void UpdateSpecialReadyVisual(float dt, Engine::UIMeter* specialBar);

	void UpdatePlayerUI(float dt);

public:

	void SetCharacterParameter(Character::ParameterComponent* parameter) { parameterComponent = parameter; };
	/// <summary>
	/// 追従対象と描画カメラ設定
	/// </summary>
	void SetFollowTarget(Engine::WorldTransform* target, Engine::Camera* camera);

	/// <summary>
	/// スペシャルゲージサイズ設定
	/// </summary>
	void SetSpecialGaugeSize(float size) { sizeSpecialGauge_ = size; }
	/// <summary>
	/// maxテクスチャ映すか
	/// </summary>
	void SetIsTextmax(bool is) { isTextMax_ = is; }
private:
	Character::ParameterComponent* parameterComponent = nullptr;
	Engine::WorldTransform* followTarget_ = nullptr;	// UI追従対象のワールドトランスフォーム
	Engine::Camera* followCamera_ = nullptr;			// ワールド座標をスクリーン座標へ変換するカメラ

	bool isTextMax_ = false;
	float sizeSpecialGauge_ = 0;
	float specialReadyEffectTimer_ = 0.0f;	// SP満タン強調用タイマー
private:
	// メータUIデータ
	struct MeterUIData {
		Vector2 pos = { 30,600 };
		Color color = { 0,1,0,0.75f };
		Color nameColor = { 0.75f,0.75f ,0.75f ,0.75f };

		float maxMeter = 100.0f;
		Vector2 offset = { 4.0f,4.0f };

		Vector2 size = { 100.0f,15.0f };
		Vector2 nameSize = { 60.0f,40.0f };
	};
	MeterUIData hpSpriteData;
	MeterUIData spSpriteData;
	MeterUIData staminaSpriteData;

	struct FollowUIData {
		Vector3 worldOffset = { 0.0f,2.0f,0.0f };	// プレイヤー左側にUIを置くための高さ基準
		Vector2 screenOffset = { 50.0f,50.0f };	// スクリーン上でメーター全体をプレイヤー左側へ寄せるオフセット
		float rowInterval = 16.0f;					// HP/SP/Staminaを縦に並べる間隔
		Vector2 hiddenPos = { -1000.0f,-1000.0f };	// 画面外やカメラ未設定時の退避座標
	};
	FollowUIData followUIData_;

	struct TextData {
		Vector2 pos = { 1120,420 };
		Vector2 pos2 = { 1120,370 };

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
	TextData evadeTextData;

	struct RBData {
		float size_ = 0.2f;
		Vector2 pos_ = { 1280 / 2,550 };
		float rotate_ = Math::DegreesToRadians(-30);
		Vector2 anchorPoint_ = { 0.5f,0.5f };
		Color color_ = { 1,1,1,0.75f };
	};
	RBData rbData_;

};

