#pragma once
#include "DirectXGame/application/base/BaseClass/Character/BaseCharacter.h"


#include "DirectXGame/application/base/Character/Player/Normal/NormalPlayerState.h"
#include "DirectXGame/application/base/UI/PlayerUI.h"

#include"DirectXGame/application/base/BaseClass/Weapon/BaseWeapon.h"
#include"DirectXGame/application/base/BaseClass/Special/BaseSpecial.h"

using PlayerStateFactory = std::function<std::unique_ptr<BasePlayerState>(BasePlayer*)>;



class FollowCamera;
class BasePlayer : public BaseCharacter
{
public:
	BasePlayer() {
		// 初期化時に状態名とその生成関数を登録
		RegisterState("Move", [](BasePlayer* p) {
			return std::make_unique<PlayerStateMove>(p);
			});
		RegisterState("Attack", [](BasePlayer* p) {
			return std::make_unique<PlayerStateAttack>(p);
			});
		RegisterState("Special", [](BasePlayer* p) {
			return std::make_unique<PlayerStateSpecial>(p);
			});
	}

	// 初期化
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;

	// 毎フレーム更新
	virtual void Update() = 0;

	virtual void DrawEffect() = 0;

	virtual void Draw2D() = 0;

	virtual void Move() = 0;
	
	virtual void Jump() = 0;

	virtual void Attack() = 0;

public:
	// フォローカメラの設定
	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera;}



	virtual PlayerUI* GetPlayerUI() = 0;

	// ヒット数の取得
	int GetHitCount() const { return weapon_->GetHitData().hitCount; }
	// 攻撃がヒットしたら加算
	void AddHit() { weapon_->GetHitData().AddHit(); };
	// ヒットタイマーのリセット
	void SetHitTime() { weapon_->GetHitData().ResetTimer(); }
	// SPゲージ加算
	void AddSpGauge(int d) { special_->AddGauge(d); };
	// SP発動可能？
	bool GetIsSpecial() const { return special_->GetIsSpecial(); }


	void ChangeState(const std::string& name) {
		auto it = stateFactoryMap_.find(name);
		if (it != stateFactoryMap_.end()) {
			stateName_ = name;
			ChangeState(it->second(this)); // unique_ptr<BasePlayerState>
		}
		else {
			assert(true);
		}
	}


	

private:

	void ChangeState(std::unique_ptr<BasePlayerState> newState) {
		if (state_) {
			state_->Exit();
		}
		state_ = std::move(newState);
		if (state_) {
			state_->Enter();
		}
	}

	void RegisterState(const std::string& name, PlayerStateFactory factory) {
		stateFactoryMap_[name] = factory;
	}
protected:
	
	FollowCamera* followCamera_;			// フォローカメラ

	bool isCreativeMode = false;			// クリエイティブモードかどうか


	std::string stateName_ = "";
	std::unique_ptr<BasePlayerState> state_;// ステート
	std::unordered_map<std::string, PlayerStateFactory> stateFactoryMap_;	// 
};

