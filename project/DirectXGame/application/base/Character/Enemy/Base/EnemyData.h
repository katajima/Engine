#pragma once

namespace Character {
	// 敵キャラクターの種類を定義する列挙型
	enum class EnemyType
	{
		kSmallMelee,	// 小型近接
		kSmallRanged,	// 小型遠距離

		kMediumMelee,	// 中型近接


		kDummy,			// ダミー
	};


	struct Enemy{

	};



	// 敵保存項目
	struct EnemyGlobalData {
		// 攻撃猶予時間
		float attackTimer = 3.0f;
		// 攻撃開始範囲
		float attackStartRadius = 10.0f;
		// 後退開始範囲
		float startRetreatingRadius = 3.0f;
		// 後退スピード
		float retreatSpeed = 1.0f;
		// 回転スピード
		float turnSpeed = 0.25f;
	};
}