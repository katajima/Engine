#pragma once
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"


#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayerState.h"
#include "DirectXGame/application/base/UI/PlayerUI.h"
#include"DirectXGame/application/base/Weapon/Base/BaseWeapon.h"
#include"DirectXGame/application/base/Special/Base/BaseSpecial.h"

// 前方宣言
class ComboEditor;


//前方宣言
class FollowCamera;
class BaseEnemy;
/// <summary>
/// プレイヤクラス
/// </summary>
class BasePlayer : public BaseCharacter
{
public:
	// 初期化
	virtual void Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, 
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) = 0;

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

	// リロード
	virtual void Reload() {};
	// コンボデータをシーケンサー適応
	virtual void ApplyComboData(ComboEditor* comboEditor) {};


public:
	// フォローカメラの設定
	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera;}

	// プレイヤUI取得
	virtual PlayerUI* GetPlayerUI() = 0;

	// SPゲージ加算
	void AddSpGauge(int d) { special_->AddGauge(d); };
	// SP発動可能？
	bool GetIsSpecial() const { return special_->GetIsSpecial(); }

protected:
	std::vector<BaseEnemy*> targetCharacters_;				// 攻撃対象キャラクターリスト
	FollowCamera* followCamera_ = nullptr;					// フォローカメラ
	bool isCreativeMode = false;							// クリエイティブモードかどうか


	// 影用オブジェクトコンポーネント
	std::unique_ptr<ObjectComponent> objectComponentShadow_ = nullptr;
};

