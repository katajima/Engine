#pragma once

namespace Character {

	// 敵保存項目
	struct EnemyGlobalData {
		// 攻撃猶予士官
		float attackTimer = 3.0f;
		// 攻撃開始範囲
		float attackStartRadius = 10.0f;
		// 後退開始範囲
		float startRetreatingRadius = 3.0f;
		// 後退スピード
		float retreatSpeed = 1.0f;
	};
}