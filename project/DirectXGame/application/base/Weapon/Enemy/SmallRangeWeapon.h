#pragma once
#include "DirectXGame/application/base/Weapon/Base/BaseWeapon.h"

class SmallRangeWeapon : public RangedWeapon
{
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
	void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, 
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) override;

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


	void Shot();

	/// <summary>
	/// 狙うターゲット指定
	/// </summary>
	/// <param name="target"></param>
	void SetTarget(const Character::BaseCharacter* target) { this->target = target; }

	/// <summary>
	/// 所有者設定
	/// </summary>
	/// <param name="player"></param>
	void SetParent(Character::BaseCharacter* player) { this->character = player; }


private:
	

	int maxTargetNum_ = 1;				// ターゲットの指定量
	Vector3 targetPos_{ 0,0,0 };		// ターゲットの位置
	float interpolation_ = 0.1f;		// 補間
	float timer_ = 0.0f;				// タイマー

	Vector3 defaultPosition_{ 0,0,0 };	// デフォルト位置
	Vector3 penetrationPos_{ 0,0,0 };		// 貫通弾位置

	ModeType modeType_ = ModeType::Normal;	// 弾のモード
};




