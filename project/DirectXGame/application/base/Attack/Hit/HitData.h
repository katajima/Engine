#pragma once
#include <DirectXgame/engine/Animation/AnimationData.h>

// 攻撃ヒットデータ構造体
struct AttackHitData
{
	int hitCount = 0;	// ヒットカウント
	Timer hitTime;		// コンボリセットまでの時間
	// 最大ヒット時間設定
	void SetmaxHitTime(float maxHitTime)
	{
		hitTime.maxT = maxHitTime; // コンボリセットまでの時間を設定
	}

	// タイマーをリセット
	void ResetTimer()
	{
		hitTime.t = 0; // タイマーをリセット
	}

	// リセット
	void Reset()
	{
		hitCount = 0;
		hitTime.t = 0;
	}

	// ヒットカウントを増やす
	void AddHit()
	{
		hitCount++;
	}

	// 更新
	void Update(float deltaTime)
	{
		hitTime.Update(deltaTime);

		if (hitTime.IsMaxOverT()) {
			Reset(); // タイマーが最大値を超えたらリセット
		}
	}
};
