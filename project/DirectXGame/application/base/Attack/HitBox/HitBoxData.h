#pragma once
#include <DirectXGame/application/base/Attack/AttackData.h>
#include <DirectXGame/engine/Collider/3d/ColliderComponent.h>

namespace HitBox {

	// 形状
	enum class Shape {
		kOBB,
		kAABB,
		kSphere,
	};

	// 使用者のタイプ
	enum class UseType {
		kPlayer,	// プレイヤー
		kEnemy,		// 敵
		kOther,		// その他
	};

	enum class SpawnType {
		kOnTime,			// 時間で発生
		kOnGround,			// 着地で発生
		kOnAir,				// 空中から発生
		kOnButtonRelease,	// ボタンを離したら
	};

	enum class ParentType {
		kParent,			// 追従型
		kIndependent,		// 独立(飛び道具や範囲攻撃など)
		kParentIndependent,	// 追従位置での独立(武器の中間位置で発生してその後独立)

		kLockOnArea,		// ターゲット位置
	};

	// 保存項目用データ
	struct GlobalData {
		Vector3 offset{};
		Vector3 size = { 1.0f,1.0f,1.0f };
		float radius = 1.0f;
	};

	// コライダーデータ
	struct CollData
	{
		///
		/// 判別用
		///

		std::string name = "";
		CollisionTag tag;
		CollisionLayer layer;
		CollisionLayer mask;

		/// <summary>
		/// フラグ
		/// </summary>

		bool isEneble = true;
		bool isLine = false;

		/// <summary>
		/// 調整
		/// </summary>

		std::string parentName = "";
		Shape shape = Shape::kOBB;
		Vector3 offset{};
		Vector3 size = { 1.0f,1.0f,1.0f };
		float radius = 1.0f;

		/// <summary>
		/// リアクション
		/// </summary>
		AttackReactionData reactionData;
	};





};