#pragma once
#include <cstdint>

namespace Character {
	// 群衆全体が採用する戦術パターン
	enum class EnemyCrowdBehaviorType {
		Rush,					// 直進型: プレイヤーへ一気に押し寄せる
		Encircle,				// 包囲型: プレイヤーの周囲を囲む
		WaveAssault,			// 波状型: 攻め込む列を時間で交代する
		Flocking,				// 群れ型: 既存の群れ移動を中心に使う
		Formation,				// 隊列型: 指定された陣形を維持する
		FlowCrowd,				// 流動型: 周囲を流れながら前へ入る
		PressureCrowd,			// 圧迫型: 密度を高めて狭く押す
		DistributedEncircle,	// 分散型: 逃げ道を残して広く包囲する
		DensityAdaptive,		// 密度制御型: 人数に応じて戦術を変える
		Spiral,					// スパイラル型: 回転しながら距離を詰める
		Hunting,				// ハンティング型: 役割分担で追い込む
	};

	// 隊列型で利用する陣形
	enum class EnemyFormationShape {
		Line,		// 横列
		Circle,		// 円形
		VShape,		// V字
		Square,		// 方陣
	};

	// ハンティング型でメンバーに順番に割り当てる役割
	enum class EnemyHuntingRole {
		Chaser,		// 追い込み
		Flanker,	// 横取り
		Ambusher,	// 待ち伏せ
		Blocker,	// 圧迫
	};

	/// <summary>
	/// 群衆グループの行動設定。スポーン時にプリセットを渡すだけで戦術を変更できる。
	/// </summary>
	struct CrowdBehaviorSettings {
		EnemyCrowdBehaviorType type = EnemyCrowdBehaviorType::Flocking;
		EnemyFormationShape formation = EnemyFormationShape::Line;
		float radius = 11.0f;			// 包囲・回転配置の基準半径
		float spacing = 3.0f;			// 隊列のメンバー間隔
		float angularSpeed = 0.7f;		// 流動・スパイラルの回転速度
		float waveInterval = 2.0f;		// 波状型で前列が交代する秒数
		uint32_t waveCount = 3;			// 波状型の列数

		/// <summary>
		/// 直進型
		/// </summary>
		/// <returns></returns>
		static CrowdBehaviorSettings Rush() {
			CrowdBehaviorSettings data{};
			data.type = EnemyCrowdBehaviorType::Rush;
			return data;
		}

		/// <summary>
		///  包囲型
		/// </summary>
		/// <param name="radius"></param>
		/// <returns></returns>
		static CrowdBehaviorSettings Encircle(float radius = 11.0f) {
			CrowdBehaviorSettings data{};
			data.type = EnemyCrowdBehaviorType::Encircle;
			data.radius = radius;
			return data;
		}

		/// <summary>
		/// 波状型
		/// </summary>
		/// <param name="interval"></param>
		/// <param name="waves"></param>
		/// <returns></returns>
		static CrowdBehaviorSettings WaveAssault(float interval = 2.0f, uint32_t waves = 3) {
			CrowdBehaviorSettings data{};
			data.type = EnemyCrowdBehaviorType::WaveAssault;
			data.waveInterval = interval;
			data.waveCount = waves;
			return data;
		}

		/// <summary>
		///  群れ型
		/// </summary>
		/// <returns></returns>
		static CrowdBehaviorSettings Flocking() {
			return {};
		}

		/// <summary>
		/// 隊列型
		/// </summary>
		/// <param name="shape"></param>
		/// Line, 横列
		/// Circle, 円形
		///	VShape,	V字
		///	Square,	方陣
		/// <returns></returns>
		static CrowdBehaviorSettings Formation(EnemyFormationShape shape) {
			CrowdBehaviorSettings data{};
			data.type = EnemyCrowdBehaviorType::Formation;
			data.formation = shape;
			return data;
		}

		/// <summary>
		/// 流動型
		/// </summary>
		/// <param name="radius"></param>
		/// <returns></returns>
		static CrowdBehaviorSettings FlowCrowd(float radius = 12.0f) {
			CrowdBehaviorSettings data{};
			data.type = EnemyCrowdBehaviorType::FlowCrowd;
			data.radius = radius;
			return data;
		}

		/// <summary>
		///  圧迫型
		/// </summary>
		/// <returns></returns>
		static CrowdBehaviorSettings PressureCrowd() {
			CrowdBehaviorSettings data{};
			data.type = EnemyCrowdBehaviorType::PressureCrowd;
			data.radius = 8.0f;
			return data;
		}

		/// <summary>
		///  分散包囲型
		/// </summary>
		/// <param name="radius"></param>
		/// <returns></returns>
		static CrowdBehaviorSettings DistributedEncircle(float radius = 14.0f) {
			CrowdBehaviorSettings data{};
			data.type = EnemyCrowdBehaviorType::DistributedEncircle;
			data.radius = radius;
			return data;
		}

		/// <summary>
		/// 人数適応型
		/// </summary>
		/// <returns></returns>
		static CrowdBehaviorSettings DensityAdaptive() {
			CrowdBehaviorSettings data{};
			data.type = EnemyCrowdBehaviorType::DensityAdaptive;
			return data;
		}

		/// <summary>
		///  スパイラル型
		/// </summary>
		/// <param name="radius"></param>
		/// <returns></returns>
		static CrowdBehaviorSettings Spiral(float radius = 15.0f) {
			CrowdBehaviorSettings data{};
			data.type = EnemyCrowdBehaviorType::Spiral;
			data.radius = radius;
			return data;
		}

		/// <summary>
		/// ハンティング型
		/// </summary>
		/// <returns></returns>
		static CrowdBehaviorSettings Hunting() {
			CrowdBehaviorSettings data{};
			data.type = EnemyCrowdBehaviorType::Hunting;
			return data;
		}
	};
}
