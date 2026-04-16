#pragma once
#include <DirectXGame/application/base/Attack/AttackData.h>
#include <DirectXGame/engine/Collider/3d/ColliderComponent.h>

namespace HitBox {

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
	// ヒットボックスヒット制限タイプ
	enum class HitContactRecord {

	};



	// 保存項目用データ
	struct GlobalData {
		Vector3 offset{};
		Vector3 size = { 1.0f,1.0f,1.0f };
		float radius = 1.0f;
	};

	// コライダーデータ
	struct CollData	{
		// 名前
		std::string name = "";
		// タグ
		CollisionTag tag;
		// レイヤー
		CollisionLayer layer;
		// マスク
		CollisionLayer mask;
		// 有効か？
		bool isEneble = true;
		// デバック用ライン描画をするか
		bool isLine = false;
		// 親子付け対象名前
		std::string parentName = "";	
		// コライダー形状
		ShapeType shape = ShapeType::kOBB;
		// コライダーの内容
		HitEffectType hitEffectType = HitEffectType::kDamageAndForce;
		// オフセット
		Vector3 offset{};
		// AABBやOBB用サイズ 
		Vector3 size = { 1.0f,1.0f,1.0f };
		// 球用サイズ
		float radius = 1.0f;
		// リアクションデータ
		HitReactionData reactionData;
	};





};