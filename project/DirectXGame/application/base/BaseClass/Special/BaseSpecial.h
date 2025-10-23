#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"

// 前方宣言
class Input;
class Entity3DManager;
class Entity2DManager;

/// <summary>
/// 必殺技クラス
/// </summary>
class BaseSpecial
{
public:
	
	///< summary>
	/// 初期化
	///</summary>
    virtual	void Initialize(Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, Camera* camera) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;

	///< summary>
	/// 描画
	///</summary>
	virtual void Draw() = 0;

public:

	// フェーズ取得
	int GetPhese() const { return phese_; }
	// フェーズセット
	void SetPhese(int phese) { phese_ = phese; }
	// 最大ゲージ取得
	void SetMaxGauge(int gauge) { maxGauge_ = gauge; }
	// ゲージ取得
	int GetGauge() const { return gauge_; };
	// ゲージセット
	void SetGauge(int gauge) { gauge_ = gauge; };
	// ゲージ追加
	void AddGauge(int gauge) { gauge_ += gauge; };


	// スペシャル
	bool GetIsSpecial() const { return isSpecial_; }
	// Inputでのbool
	bool GetIsSpecialAttack() const { return isSpecialAttack; }
	// Inputでのbool
	void SetIsSpecialAttack(bool is) { isSpecialAttack = is; }
	// アクション中か
	bool IsAction() const { return isAction_; }

	// 入力をセット
	void SetInput(Input* input) {input_ = input;};
	// 親子付け
	void SetParent(WorldTransform* parent) {};
protected:
	// ゲージ
	int gauge_ = 0;
	// 最大値
	int maxGauge_ = 1;
	// 
	bool isSpecial_ = false;
	// フェーズ
	int phese_ = 0;
	// 時間
	float time_ = 0;
	
	// クロック
	int clock_ = 1;

	// 発射ボタン
	bool isSpecialAttack = false;// RT

	// 発動中か
	bool isAction_ = false;
protected:
	Input* input_;
};

