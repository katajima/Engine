#pragma once
#include <DirectXGame/application/base/Character/Spawn/CharacterSpawnManager.h>


// 前方宣言
class Entity3DManager;
class Entity2DManager;
class GlobalVariables;


/// <summary>
/// イベントデータ構造体
/// </summary>
struct EventData {
	int phase = 0;							// イベントフェーズ
	float maxTimer = 0.0f;					// イベント最大タイマー
};

/// <summary>
/// イベントクラス
/// </summary>
class Event {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="data"></param>
	void Initialize(const EventData& data);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

private:
	EventData data_;				// イベントデータ
	bool isEventActive_ = false;	// イベントがアクティブかどうか
	float timer = 0.0f;				// イベントタイマー
	bool endFlag_ = false;			// イベント終了フラグ
	bool isEndPhase_ = false;		// 最終フェーズか

};


/// <summary>
/// ゲームのイベントを管理するクラス
/// </summary>
class GameEventController {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Entity3DManager* entity3DManager, GlobalVariables* globalVariables,  BaseCharacterManager* characterManager);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	
	void Draw() {};


	bool IsEndEvent() const { return isEndEvent_; };

private:
	std::unique_ptr<CharacterSpawnManager> characterSpawnManager_;

	bool isEndEvent_ = false;	// イベント終了フラグ
private:
	Entity3DManager* entity3DManager_ = nullptr;
	GlobalVariables* globalVariables_ = nullptr;
	BaseCharacterManager* characterManager_ = nullptr;
};
