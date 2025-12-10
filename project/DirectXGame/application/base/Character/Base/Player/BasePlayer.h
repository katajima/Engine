#pragma once
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"


#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayerState.h"
#include "DirectXGame/application/base/UI/PlayerUI.h"
#include"DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include"DirectXGame/application/base/Special/Base/BaseSpecial.h"


//前方宣言
class FollowCamera;
class BaseEnemy;
class CameraManager;
/// <summary>
/// プレイヤクラス
/// </summary>
class BasePlayer : public BaseCharacter
{
public:
	// 初期化
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;

	// 毎フレーム更新
	virtual void Update() = 0;
	// 描画エフェクト
	virtual void DrawEffect() = 0;
	// 描画2d
	virtual void Draw2D() = 0;
	// 移動
	virtual void Move() = 0;
	// ジャンプ
	virtual void Jump() = 0;
	// 攻撃(弱攻撃)
	virtual void Attack() = 0;
	// 攻撃(強攻撃)
	virtual void HeavyAttack() {};

	// ターゲットキャラクターを設定
	void SetTargetCharacters(const std::vector<BaseEnemy*>& targetCharacters) { targetCharacters_ = targetCharacters; }

public:
	// フォローカメラの設定
	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera;}

	void SetCameraManager(CameraManager* cameraManager) { cameraManager_ = cameraManager; }

	// プレイヤUI取得
	virtual PlayerUI* GetPlayerUI() = 0;

	// SPゲージ加算
	void AddSpGauge(int d) { special_->AddGauge(d); };
	// SP発動可能？
	bool GetIsSpecial() const { return special_->GetIsSpecial(); }

protected:
	std::vector<BaseEnemy*> targetCharacters_;				// 攻撃対象キャラクターリスト
	FollowCamera* followCamera_ = nullptr;					// フォローカメラ
	CameraManager* cameraManager_ = nullptr;				// カメラマネジャー
	bool isCreativeMode = false;							// クリエイティブモードかどうか
};

