#pragma once

#include "DirectXGame/application/base/Character/Enemy/Base/AI/EnemyCrowdBehaviorData.h"
#include "DirectXGame/engine/struct/Vector3.h"

namespace Character {
	class CharacterManager;

	/// <summary>
	/// ImGuiから任意の編成と行動設定を持つ群衆を生成するデバッグ機能。
	/// </summary>
	class EnemyCrowdSpawnDebugSystem {
	public:
		// デバッグウィンドウを更新し、生成ボタンが押されたときだけ敵を追加する
		void Update(CharacterManager* characterManager);

	private:
#ifdef _DEBUG
		// 画面に入力された値を、既存の群衆AIが利用する設定へまとめる
		CrowdBehaviorSettings BuildBehaviorSettings() const;
		// 選択中の敵構成をひとつの群衆グループとして生成する
		void SpawnConfiguredCrowd(CharacterManager* characterManager);
		// 現在活動中の群衆を一覧表示し、指定された群衆の退場要求を受け取る
		void DrawActiveCrowds(CharacterManager* characterManager);
#endif

		Vector3 spawnPosition_ = { 0.0f, 0.0f, 40.0f };
		Vector3 spawnAreaSize_ = { 10.0f, 0.0f, 10.0f };

		EnemyCrowdBehaviorType behaviorType_ = EnemyCrowdBehaviorType::Flocking;
		EnemyFormationShape formationShape_ = EnemyFormationShape::Line;
		float radius_ = 11.0f;
		float spacing_ = 3.0f;
		float angularSpeed_ = 0.7f;
		float waveInterval_ = 2.0f;
		int waveCount_ = 3;

		int smallMeleeCount_ = 5;
		int smallRangedCount_ = 0;
		int mediumMeleeCount_ = 0;
		int dummyCount_ = 0;

		// 手動生成した群衆が互いに混ざらないよう、生成操作ごとにIDを更新する
		int nextGroupId_ = 10000;
	};
}
