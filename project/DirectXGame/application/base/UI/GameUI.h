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

	/// <summary>
	/// プレイヤ設定
	/// </summary>
	void SetPlayer(Character::BasePlayer* player);

	/// <summary>
	/// ゲームデータ取得
	/// </summary>
	void SetGamePlayData(const Game::GameData data) { data_ = data; };

private:
	std::unique_ptr<Engine::Sprite> text_clear;
	std::unique_ptr<Engine::Sprite> text_over;
	std::unique_ptr<Engine::Sprite> text_bar;
private:
	Character::BasePlayer* player;
	Game::GameData data_;
private:
	struct ClearTextData {
		Vector2 pos_ = { 300,200 };
		Vector2 scale_ = { 100 * 1.5f,33 * 1.5f };
		float size_ = 2.0f;

		Color color_ = { 0,1,0,1 };
	};
	ClearTextData clearTextData_;


	struct HitTextData {
		Vector2 pos_ = { 1000,180 };
		int instance = 3;
		Vector2 size = { 64 * 2 / 3, 96 * 2 / 3 };
		Vector2 nameSize = { 100 * 1.5f,33 * 1.5f };
		Vector2 offset = { 20.0f,0.0f };
		Vector2 textureSize = { 64,96 };
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
		Vector2 textureSize = { 64,96 };
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
		Vector2 textureSize = { 64,96 };
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

