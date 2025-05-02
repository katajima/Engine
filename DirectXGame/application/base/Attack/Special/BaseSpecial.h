#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"


class Input;
class Entity3DManager;
class Entity2DManager;
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

	// フェーズ
	int GetPhese() const { return phese_; }
	void SetPhese(int phese) { phese_ = phese; }
	//
	void SetMaxGauge(int gauge) { maxGauge_ = gauge; }
	int GetGauge() const { return gauge_; };
	void SetGauge(int gauge) { gauge_ = gauge; };
	void AddGauge(int gauge) { gauge_ += gauge; };


	// スペシャル
	bool GetIsSpecial() const { return isSpecial_; }
	
	void SetInput(Input* input) {input_ = input;};
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

protected:
	Input* input_;
};

