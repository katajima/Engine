//#include "CrowdManager.h"
//#include <DirectXGame/engine/Math/Random.h>
//
//
//
//void CrowdAgent::Initialize(uint32_t _id, uint32_t _group, const Vector3& pos)
//{
//	id = _id; groupId = _group; 
//	position_ = pos; 
//	velocity_ = { 0,0,0 };
//	state_ = AgentState::Idle;
//}
//
//void CrowdAgent::Update(float dt, const Vector3& groupTarget, const std::vector<int>& neighborIndices, const std::vector<CrowdAgent>* allAgents)
//{
//	if (state_ == AgentState::Dead) return;
//
//
//	// 基本目標方向
//	Vector3 toTarget = groupTarget - position_;
//	float distToTarget = Length(toTarget);
//	Vector3 desired = (distToTarget > 0.001f) ? Normalize(toTarget) : Vector3{ 0,0,0 };
//	
//
//	// 簡易分離（separation）：近くの仲間とぶつからないように押しのける
//	Vector3 sep = { 0,0,0 };
//	for (int idx : neighborIndices)
//	{
//		if (idx < 0) continue;
//		const CrowdAgent& other = (*allAgents)[idx];
//		if (other.id == id) continue;
//		Vector3 toOther = position_ - other.position_;
//		float d = Length(toOther);
//		if (d < 0.001f) continue;
//		float desiredGap = radius + other.radius + 0.1f;
//		if (d < desiredGap)
//		{
//			float inv = (desiredGap - d) / desiredGap; // 0..1
//			sep = sep + Normalize(toOther) * inv; // 押しのけベクトル
//		}
//	}
//
//
//	// 回避とランダム化を少し追加して群れとしての見た目を向上
//	Vector3 steer = desired + sep * 2.0f + Vector3{ Random::RandomFloat(-0.05f,0.05f),0.0f,Random::RandomFloat(-0.05f,0.05f) };
//	steer = Normalize(steer);
//
//
//	// 状態に応じた速度制御
//	float targetSpeed = speed;
//	if (state_ == AgentState::Idle) targetSpeed *= 0.2f;
//	if (state_ == AgentState::Attack) targetSpeed *= 0.8f;
//
//
//	velocity_ = steer * targetSpeed;
//	position_ = position_ + velocity_ * dt;
//
//
//	// 攻撃クールダウン減算
//	if (attackCooldown_ > 0.0f) attackCooldown_ -= dt;
//
//
//	// シンプルな状態遷移
//	if (distToTarget < 1.2f)
//	{
//		state_ = AgentState::Attack;
//		if (attackCooldown_ <= 0.0f)
//		{
//			// 攻撃を行ったと仮定
//			attackCooldown_ = 1.0f + Random::RandomFloat(0.0f, 0.5f);
//			animIndex = 1; // 仮：攻撃アニメ
//		}
//	}
//	else
//	{
//		state_ = (Length(velocity_) > 0.1f) ? AgentState::Approach : AgentState::Idle;
//		animIndex = 0; // 仮：歩行/待機
//	}
//}
