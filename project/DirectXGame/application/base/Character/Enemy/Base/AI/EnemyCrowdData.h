#pragma once
#include <DirectXGame/engine/struct/Vector3.h>
#include <cstdint>

namespace Character {
	class BaseEnemy;

	// 群衆隊形で敵が目指す位置
	struct CrowdSlot {
		Vector3 position{};				// 隊形上の目標位置
		BaseEnemy* owner = nullptr;		// この位置を担当する敵
		BaseEnemy* commander = nullptr;	// 統率者。統率者なし群衆ではnullptr
		int groupId = 0;					// 群衆グループID
		uint32_t memberIndex = 0;		// グループ内での並び順
		bool isCommander = false;		// このスロット担当が統率者か
	};

	// プレイヤー周囲での役割ごとの移動レイヤー
	enum class EnemyCrowdLayer {
		Attack,		// 攻撃スロットへ向かう
		Approach,	// 前列へ詰める
		Standby,	// 密度を下げるため後方待機
	};

	// Separation / Alignment / Cohesion / FlowFieldを合成した移動目標
	struct EnemyFlockingSteering {
		Vector3 moveTarget{};								// 今フレーム目指す位置
		Vector3 flowDirection{};							// 合成後に進む方向
		EnemyCrowdLayer layer = EnemyCrowdLayer::Approach;	// 現在の役割
		BaseEnemy* owner = nullptr;							// この結果を使う敵
		BaseEnemy* commander = nullptr;						// 追従先の統率者
		bool isDensityLimited = false;						// 密集回避で待機中か
		bool isCommander = false;							// 自分が統率者か
		bool isTargetHeld = false;							// クールタイムにより前回目標を保持しているか
	};
}
