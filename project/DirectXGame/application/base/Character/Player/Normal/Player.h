#pragma once
#include "DirectXGame/application/base/BaseClass/Character/Player/BasePlayer.h"
#include "DirectXGame/application/base/Weapon/playerWeapon.h"
#include "DirectXGame/application/base/Special/RangeBombingSpecial.h"
#include "NormalPlayerState.h"

///< summary>
/// 自キャラ
///</summary>

class Enemy;
class Player : public BasePlayer{
public:


	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Input* input,Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables ,Vector3 position, Camera* camera) override;

	///< summary>
	/// 更新
	///</summary>
	void Update() override;

	/// <summary>
	/// エフェクトの描画
	/// </summary>
	void DrawEffect() override;

	
	void Draw2D() override;

	// 移動処理
	void Move() override;

	// ジャンプ
	void Jump() override;

	void Attack() override;

	PlayerUI* GetPlayerUI()override { return ui_.get(); };



private: 
	void InitStateMachine() override;

	// 調整項目の適用
	void ApplyGlobalVariables();

	WorldTransform worldCollider_;
	std::unique_ptr<PlayerUI> ui_;			// プレイヤー用UI
public:
	void LockOn(const std::vector<BaseEnemy*>& enemys);
};