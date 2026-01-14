#include "DashSystem.h"


void DashSystem::Initialize() {

}


// 更新
void DashSystem::Update(float dt, Engine::WorldTransform& world, Engine::RigidBodyComponent& rigid) {
	// 速度処理
	SpeedProcess(dt);
	// 状態処理
	StateProcess(dt);
	// 速度反映
	DashProcess(dt,world,rigid);
	// 重力処理
	GravityProcess(dt, rigid);
}

void DashSystem::StartDash()
{
	if (!IsCanDash()) return;	// ダッシュ不可なら抜ける

	state_ = State::kStart;	// スタート状態へ
	isDash_ = true;			// ダッシュ中フラグON
	speed_ = 0.0f;			// 速度リセット
	canDash_ = false;		// ダッシュ不可フラグON
	timer_ = 0.0f;			// タイマーリセット
}

void DashSystem::SetDirection(const Vector3& direction) {
	if (IsCanDash()) direction_ = direction; // ダッシュ可能なら方向設定
}

#pragma region Process

void DashSystem::SpeedProcess(float dt) {

	if (!isDash_) return; // ダッシュ中でなければ抜ける

	// 初速
	if (state_ == State::kStart) {
		speed_ = data_.startSpeed; // 初速設定
	}

	// 速度更新
	if (state_ == State::kStart || state_ == State::kPlay) {
		speed_ += data_.acceleration; // 加速
	}
	else if(state_ == State::kEnd){
		speed_ -= data_.friction; // 減速
	}


	if (speed_ <= 0.0f) {
		isDash_ = false;	// ダッシュ終了
		canDash_ = true; // ダッシュ可能フラグON
		speed_ = 0.0f;		// 速度リセット
		timer_ = 0.0f;		// タイマーリセット		
	}

}

void DashSystem::StateProcess(float dt){

	timer_ += dt; // タイマー更新

	if (isDash_) {
		if (state_ == State::kStart) {
			state_ = State::kPlay;	// プレイ状態へ
		}
		else if(state_ == State::kPlay){
			if (timer_ >= data_.maxTime) {
				state_ = State::kEnd;	// プレイ状態へ
			}
		}
		else {
			if( speed_ <= 0.0f ) {
				state_ = State::kNone;	// 通常状態へ
				isDash_ = false;		// ダッシュ終了
			}
		}
	}
	else {
		speed_ = 0.0f;		// 速度リセット
		timer_ = 0.0f;		// タイマーリセット
	}

}

void DashSystem::DashProcess(float dt, Engine::WorldTransform& world, Engine::RigidBodyComponent& rigid)
{
	if(isDash_) {
		// 速度ベクトル計算
		Vector3 velocity = direction_.Normalize() * speed_;
		// 速度反映
		world.translate_ += velocity * dt;
	}

	// 重力無効化
	if (!data_.isDashGravity) {
		rigid.SetIsGravity(false);
	}

}

void DashSystem::GravityProcess(float dt, Engine::RigidBodyComponent& rigid)
{
	if (isUseGravity_ == false) return;

	if (!data_.isDashGravity && isDash_) {
		rigid.SetIsGravity(false);
		rigid.Velocity() = Vector3(0, 0, 0); // 速度リセット
	}
	else {
		rigid.SetIsGravity(true);
	}
}

#pragma endregion // 処理
