#pragma once

namespace Game {
	// プレイヤーのデータ
	struct PlayerData {
		// プレイヤーの倒したカウント
		int killCount = 0;
		// プレイヤーのレベル
		int level = 0;
		// HP
		float hp = 0;
	};

	// 敵のデータ
	struct EnemyData {
		// 今まで出た敵の数
		int spawnCount = 0;
		// 現在の出現数
		int currentSpawnCount = 0;
	};

	// ステージデータ
	struct StageData {

	};

	// 進行データ
	struct FlowData {
		// 現在のウェーブ
		int currentWave = 0;
		// ゲームを始めてからの時間
		float elapsedTime = 0.0f;
	};


	/// <summary>
	/// ゲーム情報を格納したクラス
	/// </summary>
	struct GameData {
		PlayerData playerData_;
		EnemyData enemyData_;
		FlowData flowData_;
	};
}


