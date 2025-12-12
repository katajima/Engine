#include "CrowdManager.h"
#include <DirectXGame/engine/Math/Random.h>
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"



#pragma region CrowdAgent


void CrowdAgent::Initialize(uint32_t _id, uint32_t _group, const Vector3& pos)
{
	id = _id;
	groupId = _group;     // 個人Idとグループid設定
	position_ = pos;      // 位置設定
	velocity_ = { 0,0,0 }; // 速度

	// Idle 廃止 → 最初から接近
	state_ = AgentState::Approach;
}


void CrowdAgent::Update(float dt, const Vector3& groupTarget,
	const std::vector<int>& neighborIndices,
	const std::vector<CrowdAgent>* allAgents)
{
	if (isDed) return;

	position_ = owner_->GetObjectComponent()->GetWorldPosition();

	Vector3 toTarget = groupTarget - position_;
	float distToTarget = Length(toTarget);
	if (distToTarget < 0.001f) return;

	if (attackDelayTimer_ > 0.0f) {
		attackDelayTimer_ -= dt;
	}

	// ===============================
	//       ステート遷移制御
	// ===============================
	switch (state_)
	{
		// ----------- Idle 完全削除 -----------

	case AgentState::Approach:
	{
		// 一定距離に入ったら攻撃準備
		if (distToTarget < preparationAttack_) {
			state_ = AgentState::PreparationAttack;
			attackDelayTimer_ = Random::RandomFloat(2.0f, 4.0f);
		}
		break;
	}

	case AgentState::PreparationAttack:
	{
		if (attackDelayTimer_ <= 0.0f) {
			state_ = AgentState::Attack;
			owner_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Attack);
			attackCooldown_ = 1.0f + Random::RandomFloat(3.0f, 0.5f);
			animIndex = 1;
		}
		else if (distToTarget > preparationAttack_) {
			// 離れたら戻って接近し直す
			state_ = AgentState::Approach;
		}
		break;
	}

	case AgentState::Attack:
	{
		if (distToTarget > preparationAttack_) {
			state_ = AgentState::Return;
			break;
		}

		if (owner_->GetCharacterStateMachine()->GetCurrentMainState() == CharacterMainState::Move) {
			state_ = AgentState::Return;
		}
		break;
	}

	case AgentState::Return:
	{
		const CrowdGroupAgent& group = manager_->GetGroup(groupId);
		Vector3 toCenter = group.anchorCenter - position_;
		float dist = Length(toCenter);

		// Idle が無いので Approach に戻す
		//if (dist < 3.0f) {
			state_ = AgentState::Approach;
		//}
		break;
	}

	default:
		break;
	}


	// ===============================
	//       ステアリング制御
	// ===============================
	Vector3 desired = Normalize(toTarget);
	Vector3 sep = { 0,0,0 };
	Vector3 cohesion = { 0,0,0 };

	// Idle を削除したので Idle の条件を除去
	if (state_ == AgentState::Approach || state_ == AgentState::Return) {
		for (int idx : neighborIndices) {
			if (idx < 0) continue;

			const CrowdAgent& other = (*allAgents)[idx];
			if (other.id == id) continue;
			if (other.isDed) continue;

			Vector3 toOther = position_ - other.position_;
			float d = Length(toOther);
			if (d < radius * 2.0f)
				sep += Normalize(toOther) * ((radius * 2.0f - d) / radius);
		}
	}

	const CrowdGroupAgent& group = manager_->GetGroup(groupId);
	Vector3 toCenter = group.anchorCenter - position_;
	if (Length(toCenter) > 5.0f) {
		float strength = (state_ == AgentState::Return) ? 1.5f : 0.5f;
		cohesion = Normalize(toCenter) * strength;
	}

	if (state_ == AgentState::PreparationAttack) {
		float idealDist = attackRange_ * 1.5f;
		if (distToTarget < idealDist * 0.9f)
			desired = Normalize(-toTarget);
		else if (distToTarget > idealDist * 1.1f)
			desired = Normalize(toTarget);
		else
			desired = { 0,0,0 };
	}

	if (state_ == AgentState::Return) {
		Vector3 toSlot = groupTarget - position_;
		desired = Normalize(toSlot);
	}

	Vector3 steer = Normalize(desired + sep * 0.5f + cohesion * 0.8f);
	float targetSpeed = speed;

	switch (state_) {
	case AgentState::PreparationAttack:
		velocity_ = 0.0f;
		targetSpeed = 0.0f; 
		break;
	case AgentState::Attack:            targetSpeed *= 0.1f; break;
	case AgentState::Return:            targetSpeed *= 0.8f; break;
	default: break; // Approach はそのまま
	}

	velocity_ = Lerp(velocity_, steer * targetSpeed, dt * 5.0f);
	velocity_.y = 0;
	owner_->Velocity() = velocity_;
}

#pragma endregion // 個

#pragma region CrowdGroupAgent
	
void CrowdGroupAgent::UpdateCenter(const std::vector<CrowdAgent>& agents)
{
	if (memberIndices.empty()) return;
	Vector3 sum = { 0,0,0 };
	for (int idx : memberIndices)
	{
		sum = sum + agents[idx].position_;
	}
	centerPos = sum * (1.0f / (float)memberIndices.size());

	// --- 理想中心(anchorCenter)を補正 ---
	Vector3 offset = centerPos - anchorCenter;
	float dist = Length(offset);
	if (dist > 10.0f) { // グループがバラけすぎた
		// 緩やかにanchorCenterを現在位置に寄せる（重心を動かす）
		anchorCenter += Normalize(offset) * (dist - 10.0f) * 0.1f;
	}
}

Vector3 CrowdGroupAgent::CalcSlotTarget(int localIndex)
{
	int cols = 5; // 横並び数
	float spacing = 2.0f;

	int row = localIndex / cols;
	int col = localIndex % cols;

	// 中央揃え
	float offsetX = (col - (cols / 2)) * spacing;
	float offsetZ = (row)*spacing;

	// 前方方向をグループの進行方向に合わせる
	Vector3 forward = Normalize(command.targetPos - centerPos);
	Vector3 right = Normalize(Cross(Vector3{ 0,1,0 }, forward));

	Vector3 worldOffset = right * offsetX + forward * offsetZ;

	// グループ中心からの絶対座標
	return centerPos + worldOffset;
}


void CrowdGroupAgent::UpdateCommandLogic(const Vector3& playerPos)
{
	float d = Length(playerPos - centerPos);

	// --- 距離しきい値 ---
	constexpr float engageRange = 30.0f;   // 交戦開始距離
	constexpr float chaseRange = 60.0f;   // 追跡を続ける最大距離
	constexpr float giveUpRange = 100.0f;  // 完全に見失う距離

	// --- 以前の行動を考慮して状態遷移を管理 ---
	switch (command.action)
	{
	case GroupAction::Idle:
	{
		// Idle状態からプレイヤー発見（一定距離内に入ったらAdvance）
		if (d < engageRange) {
			command.action = GroupAction::Advance;
			command.targetPos = playerPos;
			command.moveSpeed = 2.2f;
		}
		break;
	}

	case GroupAction::Advance:
	{
		// 攻撃可能距離に入ったらAttack
		if (d < 12.0f) {
			command.action = GroupAction::Attack;
			command.moveSpeed = 3.0f;
		}
		// プレイヤーが遠くに離れた場合は、Idleに戻さず追跡を継続
		else if (d > chaseRange) {
			command.action = GroupAction::Retreat; // 後退または追跡解除
			command.moveSpeed = 1.0f;
		}
		command.targetPos = playerPos; // 毎フレームプレイヤー座標を追従
		break;
	}

	case GroupAction::Attack:
	{
		// 攻撃レンジを維持
		command.targetPos = playerPos;

		// プレイヤーがかなり離れたら追跡に戻る
		if (d > 20.0f) {
			command.action = GroupAction::Advance;
			command.moveSpeed = 2.5f;
		}
		break;
	}

	case GroupAction::Retreat:
	{
		// 追跡解除 → 一定距離離れたらIdleに戻る
		if (d > giveUpRange) {
			command.action = GroupAction::Idle;
			command.moveSpeed = 0.0f;
		}
		else if (d < chaseRange) {
			// 再び追撃開始
			command.action = GroupAction::Advance;
			command.moveSpeed = 2.2f;
		}
		command.targetPos = playerPos;
		break;
	}

	default:
		break;
	}
}

void CrowdGroupAgent::DistributeTargets(std::vector<CrowdAgent>& agents)
{
	for (size_t i = 0; i < memberIndices.size(); ++i)
	{
		int idx = memberIndices[i];
		if (idx < 0) continue;

		Vector3 slot = CalcSlotTarget((int)i);

		// ★前方方向（Z=Forward）
		Vector3 dirToCmd = Normalize(command.targetPos - centerPos);

		// ★スロットを部隊の進行方向に少しだけ前へ押す
		Vector3 finalTarget = slot + dirToCmd * 0.5f;

		// ★修正：エージェントにスロット目標を渡す（今まで欠落していた）
		agents[idx].slotTarget_ = finalTarget;
	}
}


#pragma endregion // グループ


#pragma region CrowdManager

int CrowdManager::CreateAgent(int groupId, BaseEnemy* ene, const Vector3 pos)
{
	CrowdAgent a;
	a.owner_ = ene;
	a.radius = 10.0f;
	a.speed = 20.0f;
	a.Initialize((uint32_t)agents.size(), (uint32_t)groupId, pos);
	a.SetCrowdManager(this);
	int idx = (int)agents.size();
	agents.push_back(std::move(a));
	groups[groupId].memberIndices.push_back(idx);
	return idx;
}

void CrowdManager::Update(float dt) {
	// グリッド更新
	grid.Clear();
	for (size_t i = 0; i < agents.size(); ++i)
	{
		grid.Insert((int)i, agents[i].position_);
	}


	// Groupごとの中心/コマンド更新
	for (auto& g : groups)
	{
		g.UpdateCenter(agents);
		g.UpdateCommandLogic(playerPos);
	}


	// 部隊ごとにメンバーの目標を配布
	for (auto& g : groups)
	{
		g.DistributeTargets(agents);
	}


	// Agent更新：近傍検索 -> Update
	std::vector<int> neighbors;
	for (size_t i = 0; i < agents.size(); ++i)
	{
		CrowdAgent& a = agents[i];

		float d = Length(a.position_ - playerPos);
		float lodFactor = (d < 15.0f) ? 1.0f : ((d < 40.0f) ? 0.5f : 0.25f);
		if (Random::RandomFloat(0.0f, 1.0f) > lodFactor) continue;

		grid.QueryNeighbors(a.position_, neighbors);

		CrowdGroupAgent& grp = groups[a.groupId];

		Vector3 finalTarget;

		// ★★★ 修正：Return 以外はプレイヤーへ向かわせる ★★★
		if (a.state_ == AgentState::Return)
		{
			// フォーメーションへ戻す
			Vector3 toCenter = grp.centerPos - a.position_;
			finalTarget = grp.centerPos + Normalize(toCenter) * 1.0f;
		}
		else
		{
			// それ以外はプレイヤーを追わせる
			finalTarget = playerPos;
		}
		// それ以外はプレイヤーを追わせる
		finalTarget = playerPos;

		a.Update(dt, finalTarget, grp.memberIndices, &agents);
	}
}

void CrowdManager::BindAgentsToEnemies(std::vector<BaseEnemy*>& enemies)
{
	for (size_t i = 0; i < agents.size() && i < enemies.size(); ++i) {
		agents[i].owner_ = enemies[i];
	}
}

void CrowdManager::UpdateAgentsToInstancing()
{
	for (auto& agent : agents) {
		if (agent.owner_ && !agent.isDed) {
			if (agent.owner_->GetHP() <= 0) {
				agent.isDed = true;
			}
		}
	}
}

#pragma endregion // 管理
