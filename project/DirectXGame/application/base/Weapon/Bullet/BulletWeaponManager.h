#pragma once



#include "BulletPlayerWeapon.h"

// 前方宣言


/// <summary>
/// BulletWeaponManagerを管理・実装するクラス。
/// </summary>
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
	void Initialize(BulletManager* bulletManager, InputSystem* inputSystem, Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables);

	/// <summary>
	/// 管理者設定
	/// </summary>
	/// <param name="player"></param>
	void SetOwner(Character::BasePlayer* player) { this->player = player; }

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
	void AddBulletWeapon(const std::string& name,const Vector3& pos,const Vector3 pos2);

	/// <summary>
	/// ターゲット設定
	/// </summary>
	/// <param name="targets"></param>
	void SetTargets(std::vector<Character::BaseEnemy*> targets);

	/// <summary>
	/// 最大ターゲット数取得
	/// </summary>
	/// <returns></returns>
	int GetMaxtargetNum();

	BulletPlayerWeapon* GetBulletWeapon(const std::string& name);


	/// <summary>
	/// 通常弾
	/// </summary>
	void Normal();


		/// <summary>
		/// 貫通弾
		/// </summary>
		void Penetration();

	/// <summary>
	/// エフェクト設定
	/// </summary>
	void SetEffect(EffectSystem* effect) { this->effect = effect; };
private:
	// 銃コンテナ
	std::map<std::string, std::unique_ptr<BulletPlayerWeapon>> bulletWeapons_;

	int maxtargetNum_ = 5; // 最大ターゲット数

	


private:
	BulletManager* bulletManager = nullptr;	// 弾管理クラス
	Engine::EntityManager* entityManager = nullptr; // エンティティ管理クラス
	InputSystem* inputSystem = nullptr;                     // 入力クラス
	Character::BasePlayer* player = nullptr;
	EffectSystem* effect = nullptr;
};