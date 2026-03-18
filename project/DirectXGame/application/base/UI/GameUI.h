#pragma once
#include "DirectXGame/application/base/UI/Base/BaseUI.h"
#include "DirectXGame/application/base/Game/Base/GameData.h"

// 前方宣言
namespace Character {
	class BasePlayer;
}

/// <summary>
/// ゲームUIクラス
/// </summary>
class GameUI : public BaseUI {
public:
	//初期化
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables) override;

	//更新
	void Update(float dt) override;

	// 描画
	void Draw() override;

	// プレイヤ設定
	void SetPlayer(Character::BasePlayer* player);

	// ゲームデータ取得
	void SetGamePlayData(const Game::GameData data) { data_ = data; };

private:
	std::unique_ptr<Engine::Sprite> text_clera;
	std::unique_ptr<Engine::Sprite> text_over;
	std::unique_ptr<Engine::Sprite> text_bar;
private:
	Character::BasePlayer* player;
	Game::GameData data_;
private:
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
		Vector2 pos_ = { 1100,50 };
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
		Vector2 pos_ = { 850,50 };
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


	struct BarTextData {
		Vector2 pos_ = { 825,20 };
		Vector2 scale_ = { 250,32 };
		float size_ = 2.0f;

		Color color_ = { 0,0,0,1 };
	};
	BarTextData barTextData_;


};

