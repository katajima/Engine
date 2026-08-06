#pragma once
#include <DirectXGame/application/base/Attack/AttackData.h>
#include"DirectXGame/engine/collider/CollisionTypeIdDef.h"


namespace HitBox {

	// ヒット処理の判定結果。受け手側でガード、無敵、耐性などを差し込むための器。
	struct HitResult {
		bool accepted = true;
		bool applyDamage = true;
		bool applyReaction = true;
		bool applyTargetHitStop = true;
		bool applySelfHitStop = true;
		bool notifyComboHit = true;
		float damageScale = 1.0f;
		HitReactionData reaction{};
	};

	// 生存タイプ
	enum class LifetimeType {
		kInfinite,   // 無期限（常時存在）
		kTimed       // 期限付き
	};
	// ヒットボックスの影響タイプ
	enum class HitEffectType {
		kDamage,        // ダメージのみ
		kForce,         // ノックバック・引き寄せなど
		kDamageAndForce // 両方
	};
	// ヒットボックスの軌道評価方式。kNoneなら従来の親Transform追従を使用する。
	enum class TrajectoryType {
		kNone,
		kBezier,
		kCatmullRom,
		kOrbit,
	};

	// 形状
	enum class ShapeType {
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
	// 出現方法
	enum class SpawnType {
		kOnTime,			// 時間で発生
		kOnGround,			// 着地で発生
		kOnAir,				// 空中から発生
		kOnButtonRelease,	// ボタンを離したら
	};
	// 親子付けタイプ
	enum class ParentType {
		kParent,			// 追従型
		kIndependent,		// 独立(飛び道具や範囲攻撃など)
		kParentIndependent,	// 追従位置での独立(武器の中間位置で発生してその後独立)

		kLockOnArea,		// ターゲット位置
	};

	// 保存項目ヒットボックス
	struct GlobalHitBox {
		// ヒットボックスの生成タイミング時間
		float windowStart = 0.0f;
		// ヒットボックスの生存時間
		float lifeTime = 0.5f;
		// ヒット記録を使用するか（使用した場合連続ヒットしない）
		bool useContactRecord = true;
		/// 親子付け設定  ///　 
		std::string parentName = "なし";
		// オフセット
		Vector3 offset = { 0.0f,0.0f,0.0f };

		// ヒットボックス使用者タイプ
		HitBox::UseType useType = HitBox::UseType::kPlayer;
		// ヒットボックスの発生条件タイプ
		HitBox::SpawnType spawnType = HitBox::SpawnType::kOnTime;
		// ヒットボックス依存先タイプ
		HitBox::ParentType dependenceType = HitBox::ParentType::kParent;
		// ヒットボックス生存タイプ
		HitBox::LifetimeType lifetimeType = HitBox::LifetimeType::kTimed;
		// ヒットボックス影響タイプ
		HitBox::HitEffectType hitEffectType = HitBox::HitEffectType::kDamageAndForce;
		// ヒットボックス形状
		HitBox::ShapeType shapeType = HitBox::ShapeType::kOBB;
		// コライダーサイズ
		Vector3 colliderSize = { 1.0f,1.0f,1.0f };
		// 球半径
		float radius = 1.0f;

		// 軌道評価方式
		TrajectoryType trajectoryType = TrajectoryType::kNone;
		// 軌道を評価する時間。0以下ならヒットボックスの生存時間を使用する。
		float trajectoryDuration = 0.0f;
		// Bezier／Catmull-Romで使用するローカル制御点。生成時の向きを基準にする。
		Vector3 trajectoryPoint0 = { 0.0f, 0.0f, 0.0f };
		Vector3 trajectoryPoint1 = { 0.0f, 0.0f, 2.0f };
		Vector3 trajectoryPoint2 = { 0.0f, 0.0f, 4.0f };
		Vector3 trajectoryPoint3 = { 0.0f, 0.0f, 6.0f };
		// Orbitで使用する中心オフセット、半径、高さ、開始角、終了角。
		Vector3 trajectoryOrbitCenter = { 0.0f, 0.0f, 0.0f };
		float trajectoryOrbitRadius = 2.0f;
		float trajectoryOrbitHeight = 0.0f;
		float trajectoryOrbitStartAngle = 0.0f;
		float trajectoryOrbitEndAngle = 6.2831853f;
		// タグ
		CollisionTag tag = CollisionTag::None;
		// レイヤー
		CollisionLayer layer = CollisionLayer::ALL;
		// マスク
		CollisionLayer mask = CollisionLayer::ALL;
		// 有効か？
		bool isEneble = true;
		// デバック用ライン描画をするか
		bool isLine = false;
		// 同じ攻撃内でコライダー別ヒットを許可するか。
		bool recordPerCollider = true;
	};

	// コライダーデータ
	struct CollData	{
		// 名前
		std::string name = "";
		// ヒットボックスデータ
		GlobalHitBox hitBoxData;
		// リアクションデータ
		HitReactionData reactionData;
	};
};
