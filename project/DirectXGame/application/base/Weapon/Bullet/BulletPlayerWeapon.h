#pragma once
#include "DirectXGame/application/base/Weapon/Base/RangedWeapons.h"


// 前方宣言
class BasePlayer;
class BaseEnemy;
class Effect;



class BulletPlayerWeapon : public RangedWeapon {
public:

	enum class ModeType
	{
		Normal,			// 通常弾
		Penetration,	// 貫通弾
		Count,
	};


	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) override;

	///< summary>
	/// 更新
	///</summary>
	void Update() override;

	/// <summary>
	/// エフェクトの描画
	/// </summary>
	void DrawEffect() override;

	/// <summary>
	/// 描画2d
	/// </summary>
	void Draw2D() override;

public:

	/// <summary>
	/// 狙うターゲット指定
	/// </summary>
	/// <param name="target"></param>
	void SetTarget(BaseEnemy* target) { target_ = target; }

	/// <summary>
	/// プレイヤー設定
	/// </summary>
	/// <param name="player"></param>
	void SetParent(BasePlayer* player) { player_ = player; }


	int MaxTargetNum() const { return maxTargetNum_; }


	/// <summary>
	/// 発射
	/// </summary>
	void Shoot();

	// エフェクト設定
	void SetEffect(Effect* effect) { effect_ = effect; };

	// モードタイプ設定
	void SetModeType(ModeType mode) { modeType_ = mode; }
	

	// 
	void SetModePenetrationPos(Vector3 pos) { penetrationPos_ = pos; }

private:
	WorldTransform bulletTransform_;

	WorldTransform bulletMuzzleTransform_;
	WorldTransform bulletCartridgeTransform_;

	int maxTargetNum_ = 1;				// ターゲットの指定量
	Vector3 targetPos_{ 0,0,0 };		// ターゲットの位置
	float interpolation_ = 0.1f;		// 補間
	float timer_ = 0.0f;				// タイマー

	Vector3 defaultPosition_{ 0,0,0 };	// デフォルト位置
	Vector3 penetrationPos_{ 0,0,0 };		// 貫通弾位置

	ModeType modeType_ = ModeType::Normal;	// 弾のモード
private:
	Effect* effect_ = nullptr;
	BaseEnemy* target_ = nullptr;
	BasePlayer* player_ = nullptr;
};
