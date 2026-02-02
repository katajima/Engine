#pragma once
#include "DirectXGame/application/base/UI/Base/BaseUI.h"
#include "DirectXGame/application/base/Game/GameData.h"

// 前方宣言
class BasePlayer;

/// <summary>
/// ゲームUIクラス
/// </summary>
class GameUI : public BaseUI
{
public:
	//初期化
	void Initialize(Engine::Input* input, Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables) override;

	//更新
	void Update() override;

	// 描画
	void Draw() override;

	// プレイヤ設定
	void SetPlayer(BasePlayer* player);

	// ゲームデータ取得
	void SetGamePlayData(const GamePlayData data) { data_ = data; };

private:
	std::unique_ptr<Engine::Sprite> text_clera;
	std::unique_ptr<Engine::Sprite> text_over;
private:
	BasePlayer* player_;
	GamePlayData data_;

private:

	Vector2 boardPos = { 100,100 };
	Vector2 boardSize = { 400,400 };

	Vector2 sliderPos = { 30,50 };

	struct AttackTextData {
		Vector2 pos_ = { 1120,520 };
		Vector2 pos2_ = { 1120,470 };


		Vector2 anchorPoint_ = { 0.5f,0.5f };
		Vector2 anchorPoint2_ = { 0.0f,0.5f };

		Vector2 size_ = { 64.0f,64.0f };
		Vector2 size2_ = { 48.0f,48.0f };

		float offset_ = 20.0f;
	};
	AttackTextData attackTextData_;
	
	struct SpTextData {
		Vector2 pos_ = { 1120,570 };

		Vector2 anchorPoint_ = { 0.5f,0.5f };
		Vector2 anchorPoint2_ = { 0.0f,0.5f };

		Vector2 size_ = { 64.0f,64.0f };
		Vector2 size2_ = { 48.0f,48.0f };

		float offset_ = 20.0f;

		Color color_ = { 0.75f,0.75f ,0.75f ,1.0f };
	};
	SpTextData spTextData;

	struct JumpTextData {
		Vector2 pos_ = { 1120,620 };

		Vector2 anchorPoint_ = { 0.5f,0.5f };
		Vector2 anchorPoint2_ = { 0.0f,0.5f };

		Vector2 size_ = { 64.0f,64.0f };
		Vector2 size2_ = { 48.0f,48.0f };

		float offset_ = 20.0f;

		Color color_ = { 0.75f,0.75f ,0.75f ,1.0f };
	};
	JumpTextData jumpTextData;

	struct DashTextData {
		Vector2 pos_ = { 1120,670 };

		Vector2 anchorPoint_ = { 0.5f,0.5f };
		Vector2 anchorPoint2_ = { 0.0f,0.5f };

		Vector2 size_ = { 64.0f,64.0f };
		Vector2 size2_ = { 48.0f,48.0f };

		float offset_ = 20.0f;

		Color color_ = { 0.75f,0.75f ,0.75f ,1.0f };
	};
	DashTextData dashTextData;


	struct CleraTextData {
		Vector2 pos_ = { 300,200 };
		Vector2 scale_ = { 100 * 1.5f,33 * 1.5f };
		float size_ = 2.0f;
		
		Color color_ = { 0,1,0,1 };
	};
	CleraTextData cleraTextData_;


	struct HitTextData {
		Vector2 pos_ = { 950,100 };
		int instance = 3;
		Vector2 size = { 64 * 2 / 3, 96 * 2 / 3 };
		Vector2 nameSize = { 100 * 1.5f,33 * 1.5f };
		Vector2 offset = { 20.0f,0.0f };
		Vector2 textuerSize = { 64,96 };
		float countmax = 999;

		
		Color color_ = { 0.9f,0.9f ,0.9f ,1.0f };
		Color nameColor_ = { 0.9f,0.9f ,0.9f ,1.0f };
	};
	HitTextData hitTextData;

	struct WaveTextData {
		Vector2 pos_ = { 650,50 };
		int instance = 1;
		Vector2 size = { 64 * 2 / 3, 96 * 2 / 3 };
		Vector2 nameSize = { 100 * 1.5f,33 * 1.5f };
		Vector2 offset = { 20.0f,0.0f };
		Vector2 textuerSize = { 64,96 };
		float countmax = 9;


		Color color_ = { 0.9f,0.9f ,0.9f ,1.0f };
		Color nameColor_ = { 0.9f,0.9f ,0.9f ,1.0f };
	};
	WaveTextData waveTextData;
	
	struct WaveTimeTextData {
		Vector2 pos_ = { 400,50 };
		int instance = 3;
		Vector2 size = { 64 * 2 / 3, 96 * 2 / 3 };
		Vector2 nameSize = { 64 ,64  };
		Vector2 offset = { 20.0f,0.0f };
		Vector2 textuerSize = { 64,96 };
		float countmax = 999;


		Color color_ = { 0.9f,0.9f ,0.9f ,1.0f };
		Color nameColor_ = { 0.9f,0.9f ,0.9f ,1.0f };
	};
	WaveTimeTextData secondTextData;



};

