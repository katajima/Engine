#pragma once
#include "ProjectileFunction.h"
#include "DirectXGame/engine/Collider/3d/ColliderComponent.h"
// 前方宣言
namespace Character {
	class BaseCharacter;
}
// 発射物関連のデータや関数をまとめる名前空間
namespace Projectile {

	struct ProjectileSpawnInfo
	{
		Vector3 position{};
		Vector3 direction{};   // 正規化済み推奨
		Vector3 scale = { 1.0f,1.0f,1.0f };
		Character::BaseCharacter* owner = nullptr;
		Character::BaseCharacter* target = nullptr;

		float powerScale = 1.0f;
	};

	// 発射物の種類
	enum class Type {
		Normal,     // 通常弾
		Homing,     // ホーミング弾
		Laser,      // レーザー
		Shotgun,    // ショットガン
		Rapid,      // 連射弾
		Penetration,// 貫通弾 
	};

	// 発射物の移動タイプ
	enum class ProjectileMoveType {
		Straight,   // 直線
		Homing,     // ホーミング
		Parabola,   // 放物線
		Stay,       // 維持
	};

	// 衝突タイプ
	enum class ProjectileHitType {
		Destroy,    // 破壊
		Penetrate,  // 貫通
		Explode,    //爆発
		Bounce,     //跳ね返り
	};

	struct ProjectileParam
	{
		// 発射物の名前（識別用）
		std::string name = "";
		// モデル名
		std::string modelName = "";
		// テクスチャ名
		std::string textureName = "";


		// 移動速度
		float speed = 0.0f;
		// 生存時間
		float maxLifeTime = 0.0f;
		// コライダの半径
		float radius = 0.5f;
		// ダメージ量
		float damage = 0.0f;
		// タイプ
		Type type = Type::Normal;
		// 移動タイプ
		ProjectileMoveType moveType = ProjectileMoveType::Straight;
		// 衝突タイプ
		ProjectileHitType hitType = ProjectileHitType::Destroy;
		// 衝突タグ
		CollisionTag collisionTag = CollisionTag::None;
		// 衝突レイヤー
		CollisionLayer collisionLayer = CollisionLayer::Default;
		// 衝突マスク
		uint32_t collisionMask = 0xFFFFFFFF; // ビットで衝突対象を指定（全部と当たる）

		// 貫通するか
		bool isPierce = false;
		// 貫通する場合の最大貫通数
		int maxPierceCount = 0;
		// 跳ね返り回数
		bool enableGravity = false;
		// 重力の強さ
		float gravityScale = 1.0f;
		// ホーミングの有効化
		bool enableHoming = false;
		// ホーミングの強さ
		float homingStrength = 0.0f;
		// ホーミングの追尾範囲
		float homingRange = 0.0f;
		// トレイルエフェクトを生成するか
		bool generateTrailEffect = false;
		// トレイルエフェクトの持続時間
		float trailEffectLifeTime = 0.0f;
		
		// 爆発の持続時間
		float explosionLifeTime = 0.0f;
		// 爆発の半径
		float explosionRadius = 0.0f;
		// 爆発のダメージ
		float knockBackPower = 0.0f;
	};


}

