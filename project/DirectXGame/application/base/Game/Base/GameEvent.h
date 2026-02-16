#pragma once
#include "GameData.h"

namespace Game {

	/// <summary>
	/// ゲームイベントの変更するタイプ
	/// </summary>
	enum class GameEventChangeType {
		kTime,	// 時間経過で
		kKill,	// 全滅させたら
		kTake,	// 何かを取得したら
	};

	/// <summary>
	/// ゲームイベント内での時間に関する構造体
	/// </summary>
	struct GameEventTime {
		// 最大経過時間(次のゲームイベントに移行する時間)
		float max = 1.0f;
		// 時間
		float time = 0.0f;
	};


	/// <summary>
	/// 倒して変わるタイプ
	/// </summary>
	enum class GameEvemtKillChangeType {
		kAll,	// 全て
		kCount,	// 一定数
	};

	/// <summary>
	/// ゲームイベント内での
	/// </summary>
	struct GameEventKillChange
	{
		// 倒して変更するタイプ
		GameEvemtKillChangeType type;
		// 倒すキャラクターの数
		int* killCharacterCount;
	};



	/// <summary>
	/// ゲームイベント
	/// </summary>
	class GameEvent {
	public:

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();

		/// <summary>
		/// 更新
		/// </summary>
		/// <param name="dt"></param>
		void Update(float dt);

	public:
		// 終了したか
		bool IsFinish();
		// ゲームイベントの変更するタイプ取得
		GameEventChangeType GetChangeType() const { return changeType_; }
	private:
		// ゲームイベントの変更するタイプ
		GameEventChangeType changeType_;
		// ゲームイベント内での時間に関する構造体
		GameEventTime time_;
		int* count = nullptr;


		// 終了下か
		bool isFinish_ = false;
	};
}