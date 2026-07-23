#pragma once
#include "DirectXGame/application/base/Attack/AttackData.h"
#include <DirectXGame/application/base/Attack/HitBox/HitBoxSystem.h>

// 発射物
enum class ProjectileType {
	NORMAL,		// 通常弾
	HOMING,		// ホーミング弾
	LASER,		// レーザー
	SHOTGUN,	// ショットガン
	RAPID,		// 連射弾
	PENETRATION,// 貫通弾 
};



// 弾の種類
enum class BulletType
{
	kPlayerMissile,	// プレイヤーミサイル
	kRangeBombingSpecial,	// レンジボミングスペシャル
	kPlayerBullet,	// プレイヤー弾
	kPlayerStan,			// スタン
	kEnemyBullet,

};


/// <summary>
/// 弾の情報構造体
/// </summary>
struct BulletInfo {
	Vector3 position;			// 位置
	Vector3 targetPos;			// 目標位置
	float speed;				// 速度
	float damage;				// ダメージ量
	ProjectileType type;		// 弾の種類
};

// 弾データ
/// <summary>
/// BulletDataを管理・実装するクラス。
/// </summary>
class BulletData {
public:
	// 初期化
	void Initialize(HitBox::UseType useType, ProjectileType projectileType ,const HitBox::CollData& hitBoxData);




	// ヒットボックスデータ
	struct Data {
		float hitBpxWindowStart_ = 1.0f;		// ヒットボックス生成スタート
		float lifeTime_ = 1.0f;					// ヒットボックス生存時間
		// ヒットボックス使用者タイプ
		HitBox::UseType hitBoxUseType_;
		// ヒットボックス依存先タイプ
		HitBox::ParentType dependenceType_ = HitBox::ParentType::kParent;
		// オフセット
		Vector3 offset_{};
	};
	
private:
	// 発射物タイプ
	ProjectileType projectileType_;
	// ヒットボックスデータ
	HitBox::CollData hitBoxdata_;
	//
	Data data_;
private:

};