#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"EffectModule.h"


// 前方宣言
class Entity3DManager;
class Entity2DManager;
class GlobalVariables;
class Audio;


/// <summary>
/// エフェクトのフィールド基底クラス
/// </summary>
class EffectField {
public:

	// 初期化
	void Initialize(Entity3DManager* entity3DManager);

	// 更新
	void Update();

	// 描画
	void Draw();

	// エフェクト発動
	void Effect(BaseCharacter* character);

	// コライダコンポーネント取得
	ColliderComponent* GetColliderComponent() { return colliderComponent_.get(); }

	// ダメージモジュール使用
	void UseDamageModule();
	// ノックバックモジュール使用
	void UseKnockbackModule();
	// ステート変更モジュール使用
	void UseChangeStateModule();
public:

	// 削除フラグ取得
	bool GetDeleteFlag() const { return deleteFlag_; }

	// ダメージモジュール取得
	DamageModule* GetDamageModule() { return damage_.get(); }
	// ノックバックモジュール取得
	KnockbackModule* GetKnockbackModule() { return knockback_.get(); }
	// ステート変更モジュール取得
	ChangeStateModule* GetChangeStateModule() { return changeState_.get(); }
protected:
	// 固有の初期化
	virtual void InitializeUniqe() {};

	// 固有の更新
	virtual void UpdateUniqe() {};

	// 固有の描画
	virtual void DrawUniqe() {};
private:
	// コライダーコンポーネント
	std::unique_ptr<ColliderComponent> colliderComponent_;


	float time_ = 0.0f;			// 経過時間
	float deleteTime_ = 0.0f;	// 消滅時間
	bool deleteFlag_ = false;	// 消滅フラグ

	std::unique_ptr<DamageModule> damage_;			// ダメージ
	std::unique_ptr<KnockbackModule> knockback_;	// ノックバック
	std::unique_ptr<ChangeStateModule> changeState_;// ステート変更
private: // 貰い物
	Entity3DManager* entity3DManager_ = nullptr;	// 3Dエンティティマネージャー
	Entity2DManager* entity2DManager_ = nullptr;	// 2Dエンティティマネージャー
	GlobalVariables* globalVariables_ = nullptr;	// グローバル変数
	Audio* audio_ = nullptr;
};


