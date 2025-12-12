#pragma once
#include <DirectXGame/application/base/Character/Spawn/CharacterSpawnManager.h>
#include "GameWave.h"

// 前方宣言
namespace Engine {
	class Entity3DManager;
	class Entity2DManager;
	class GlobalVariables;
}

/// <summary>
/// ゲームのイベントを管理するクラス
/// </summary>
class GameEventController {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables,  BaseCharacterManager* characterManager);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt);

	
	void Draw() {};


	bool IsEndEvent() const { return isEndEvent_; };

private:

	// キャラクター出現管理クラス
	std::unique_ptr<CharacterSpawnManager> characterSpawnManager_ = nullptr;
	// ウェーブ管理クラス
	std::unique_ptr<WaveManager> waveManager_ = nullptr;

	bool isEndEvent_ = false;	// イベント終了フラグ
private:
	Engine::Entity3DManager* entity3DManager_ = nullptr;
	Engine::GlobalVariables* globalVariables_ = nullptr;
	BaseCharacterManager* characterManager_ = nullptr;
};
