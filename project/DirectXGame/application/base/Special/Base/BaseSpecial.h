#pragma once
#include "SpecialData.h"

#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"

// 前方宣言
namespace Engine {
	class Input;
	class Entity3DManager;
	class Entity2DManager;
}
namespace Character {
	class BaseCharacter;
}
/// <summary>
/// 必殺技クラス
/// </summary>
class BaseSpecial
{
public:
	
	///< summary>
	/// 初期化
	///</summary>
    virtual	void Initialize(Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, Engine::Camera* camera) = 0;

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
	int GetPhese() const { return data_.phase_; }
	// フェーズセット
	void SetPhese(int phese) { data_.phase_ = phese; }
	// 最大ゲージ取得
	void SetMaxGauge(int gauge) { data_.maxGauge_ = gauge; }
	// ゲージ取得
	int GetGauge() const { return data_.gauge_; };
	// ゲージセット
	void SetGauge(int gauge) { data_.gauge_ = gauge; };
	// ゲージ追加
	void AddGauge(int gauge) { data_.gauge_ += gauge; };
	// スペシャル
	bool GetIsSpecial() const { return data_.isUse_; }
	// アクション中か
	bool IsAction() const { return data_.isAction_; }
	// 移動可能か
	bool GetIsMove() const { return data_.isMove_; };
	// データ取得
	SpecialData GetData() const { return data_; }
	// 時間取得
	float GetTime();
public: // 貰いもの
	//持ち主設定
	void SetOwner(Character::BaseCharacter* owner) { this->owner = owner; }
	// 入力をセット
	void SetInput(Engine::Input* input) {this->input = input;};
	// 親子付け
	void SetParent(Engine::WorldTransform* parent) {};
protected:
	// データ
	SpecialData data_;
	// 時間
	float time_ = 0;
protected:
	Character::BaseCharacter* owner = nullptr;			// オーナー
	Engine::Input* input;					// 入力
};

