#pragma once



#include "BulletPlayerWeapon.h"

// 前方宣言
class BulletManager;
class Entity3DManager;
class Entity2DManager;
class Effect;

class BulletWeaponManager
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="bulletManager"></param>
	/// <param name="input"></param>
	/// <param name="entity3DManager"></param>
	/// <param name="entity2DManager"></param>
	/// <param name="globalVariables"></param>
	void Initialize(BulletManager* bulletManager, Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables);

	/// <summary>
	/// 管理者設定
	/// </summary>
	/// <param name="player"></param>
	void SetOwner(BasePlayer* player) { player_ = player; }

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 武器追加
	/// </summary>
	/// <param name="name"></param>
	/// <param name="pos"></param>
	void AddBulletWeapon(const std::string& name,const Vector3& pos);

	/// <summary>
	/// ターゲット設定
	/// </summary>
	/// <param name="targets"></param>
	void SetTargets(std::vector<BaseEnemy*> targets);

	/// <summary>
	/// 最大ターゲット数取得
	/// </summary>
	/// <returns></returns>
	int GetMaxtargetNum();

	BulletPlayerWeapon* GetBulletWeapon(const std::string& name);


	// エフェクト設定
	void SetEffect(Effect* effect) { effect_ = effect; };
private:
	// 銃コンテナ
	std::map<std::string, std::unique_ptr<BulletPlayerWeapon>> bulletWeapons_;

	int maxtargetNum_ = 5; // 最大ターゲット数



private:
	BulletManager* bulletManager_ = nullptr;	// 弾管理クラス
	Entity3DManager* entity3DManager_ = nullptr; // 3Dエンティティ管理クラス
	Entity2DManager* entity2DManager_ = nullptr; // 2Dエンティティ管理クラス
	Input* input_ = nullptr;                     // 入力クラス
	BasePlayer* player_;
	Effect* effect_;
};