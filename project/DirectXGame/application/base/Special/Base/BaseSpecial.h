#pragma once
#include "SpecialData.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"
#include "DirectXGame/application/base/Input/InputSystem.h"

// 前方宣言
namespace Engine {
	class Input;
	class EntityManager;
	class Camera;
}
//
namespace Character {
	class BaseCharacter;
}
class BulletSpawn;
class FollowCamera;


/// <summary>
/// 必殺技クラス
/// </summary>
class BaseSpecial
{
public:
	
	///< summary>
	/// 初期化
	///</summary>
    virtual	void Initialize(Engine::EntityManager* entity3DManager, Engine::Camera* camera) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;

	///< summary>
	/// 描画
	///</summary>
	virtual void Draw() = 0;

public:

	/// <summary>
	/// フェーズ取得
	/// </summary>
	int GetPhese() const { return data_.phase_; }
	/// <summary>
	/// フェーズセット
	/// </summary>
	void SetPhese(int phese) { data_.phase_ = phese; }
	/// <summary>
	/// 最大ゲージ取得
	/// </summary>
	void SetMaxGauge(int gauge) { data_.maxGauge_ = gauge; }
	/// <summary>
	/// ゲージ取得
	/// </summary>
	int GetGauge() const { return data_.gauge_; };
	/// <summary>
	/// ゲージセット
	/// </summary>
	void SetGauge(int gauge) { data_.gauge_ = gauge; };
	/// <summary>
	/// ゲージ追加
	/// </summary>
	void AddGauge(int gauge) { data_.gauge_ += gauge; };
	/// <summary>
	/// スペシャル
	/// </summary>
	bool GetIsSpecial() const { return data_.isUse_; }
	/// <summary>
	/// アクション中か
	/// </summary>
	bool IsAction() const { return data_.isAction_; }
	/// <summary>
	/// 移動可能か
	/// </summary>
	bool GetIsMove() const { return data_.isMove_; };
	/// <summary>
	/// データ取得
	/// </summary>
	SpecialData GetData() const { return data_; }
	/// <summary>
	/// 時間取得
	/// </summary>
	float GetTime();
public: // 貰いもの
	/// <summary>
	/// 持ち主設定
	/// </summary>
	void SetOwner(Character::BaseCharacter* owner) { this->owner = owner; }
	/// <summary>
	/// 入力をセット
	/// </summary>
	void SetInputSystem(InputSystem* inputSystem) {this->inputSystem = inputSystem;};
	/// <summary>
	/// 親子付け
	/// </summary>
	void SetParent(Engine::WorldTransform* parent) {};
protected:
	// データ
	SpecialData data_;
	// 時間
	float time_ = 0;
protected:
	Character::BaseCharacter* owner = nullptr;			// オーナー
	InputSystem* inputSystem = nullptr;					// 入力
};

