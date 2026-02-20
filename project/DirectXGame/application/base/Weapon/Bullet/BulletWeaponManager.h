#pragma once



#include "BulletPlayerWeapon.h"

// 前方宣言
class BulletManager;
namespace Engine {
	class Entity3DManager;
	class Entity2DManager;
}
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
	void Initialize(BulletManager* bulletManager, InputSystem* inputSystem, Engine::Entity3DManager* entity3DManager,
		Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables);

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

	// エフェクト設定
	void SetEffect(Effect* effect) { this->effect = effect; };
private:
	// 銃コンテナ
	std::map<std::string, std::unique_ptr<BulletPlayerWeapon>> bulletWeapons_;

	int maxtargetNum_ = 5; // 最大ターゲット数

	


private:
	BulletManager* bulletManager = nullptr;	// 弾管理クラス
	Engine::Entity3DManager* entity3DManager = nullptr; // 3Dエンティティ管理クラス
	Engine::Entity2DManager* entity2DManager = nullptr; // 2Dエンティティ管理クラス
	InputSystem* inputSystem = nullptr;                     // 入力クラス
	Character::BasePlayer* player = nullptr;
	Effect* effect = nullptr;
};