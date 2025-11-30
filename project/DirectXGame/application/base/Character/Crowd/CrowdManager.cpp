#include "CrowdManager.h"
#include <DirectXGame/engine/Math/Random.h>
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"



#pragma region CrowdAgent

void CrowdAgent::Initialize(uint32_t _id, uint32_t _group, const Vector3& pos)
{
	id = _id; groupId = _group;	// 個人Idとグループid設定
	position_ = pos;	// 位置設定
	velocity_ = { 0,0,0 };	// 速度
	state_ = AgentState::Idle;	// 待機状態に
}

void CrowdAgent::Update(float dt, const Vector3& groupTarget,
	const std::vector<int>& neighborIndices,
	const std::vector<CrowdAgent>* allAgents)
{
	// 死んでいるなら早期リターン
	if (isDed) return;
	// 位置取得
	position_ = owner_->GetObjectComponent()->GetWorldPosition();

	// ターゲット
	Vector3 toTarget = groupTarget - position_;
	// ターゲット距離
	float distToTarget = Length(toTarget);
	if (distToTarget < 0.001f) return;

	// 攻撃準備タイマー更新
	if (attackDelayTimer_ > 0.0f) {
		attackDelayTimer_ -= dt;
	}

	// ===============================
	//       ステート遷移制御
	// ===============================
	switch (state_)
	{
	case AgentState::Idle:	// 待機
	{	
		// 範囲内の入ったら
		if (distToTarget < engageDistance_) {
			state_ = AgentState::Approach;	// 接近に移行
		}
		break;
	}

	case AgentState::Approach:	// 接近
	{
		// 範囲内に入ったら
		if (distToTarget < attackRange_ * 3.0f) {
			state_ = AgentState::PreparationAttack;	// 攻撃準備に入る
			attackDelayTimer_ = Random::RandomFloat(1.0f, 3.0f);
		}
		else if (distToTarget > engageDistance_ * 1.5f) {	// 範囲外なら
			state_ = AgentState::Idle;	// 待機に戻る
		}
		break;
	}

	case AgentState::PreparationAttack:	// 攻撃準備
	{	
		// 攻撃に移行
		if (attackDelayTimer_ <= 0.0f) {
			state_ = AgentState::Attack; // 攻撃
			owner_->GetCharacterStateMachine()->ChangeState(CharacterMainState::Attack);// 攻撃ステート移行
			attackCooldown_ = 1.0f + Random::RandomFloat(0.0f, 0.5f);
			animIndex = 1;
		}
		else if (distToTarget > attackRange_ * 3.0f) {	// 範囲外なら 
			state_ = AgentState::Approach;	// 接近
		}
		break;
	}

	case AgentState::Attack:	// 攻撃
	{
		if (distToTarget > attackRange_ * 1.5f) {
			state_ = AgentState::Return; // ★攻撃後はフォーメーションへ戻る
			break;
		}

		// 攻撃モーション終了時も戻る
		if (owner_->GetCharacterStateMachine()->GetCurrentMainState() == CharacterMainState::Move) {
			state_ = AgentState::Return; // ★追加
		}
		break;
	}

	// ★追加：Return（フォーメーションへ戻る）
	case AgentState::Return:
	{
		const CrowdGroupAgent& group = manager_->GetGroup(groupId);
		Vector3 toCenter = group.anchorCenter - position_;
		float dist = Length(toCenter);

		// 中心付近まで戻ったら待機状態へ
		if (dist < 3.0f) {
			state_ = AgentState::Idle;
		}
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


	// --- 分離（Separation） ---
	if (state_ == AgentState::Approach || state_ == AgentState::Idle || state_ == AgentState::Return) {
		for (int idx : neighborIndices) {
			if (idx < 0) continue;			// idxが0以下なら次へ

			const CrowdAgent& other = (*allAgents)[idx];
			if (other.id == id) continue;	// idが同じなら次へ
			if (other.isDed) continue;		// 死んでいたら次へ


			Vector3 toOther = position_ - other.position_;
			float d = Length(toOther);
			if (d < radius * 2.0f) sep += Normalize(toOther) * ((radius * 2.0f - d) / radius);
		}
	}


	// --- 凝集（Cohesion）：Return時は強めに ---
	const CrowdGroupAgent& group = manager_->GetGroup(groupId);
	Vector3 toCenter = group.anchorCenter - position_;
	if (Length(toCenter) > 5.0f) {
		float strength = (state_ == AgentState::Return) ? 1.5f : 0.5f; // ★Return時は強め
		cohesion = Normalize(toCenter) * strength;
	}

	// --- 攻撃準備中は距離維持 ---
	if (state_ == AgentState::PreparationAttack) {
		float idealDist = attackRange_ * 1.5f;
		if (distToTarget < idealDist * 0.9f) desired = Normalize(-toTarget);
		else if (distToTarget > idealDist * 1.1f) desired = Normalize(toTarget);
		else desired = { 0,0,0 };
	}

	// Return時はグループ中心へ強く引っ張る
	if (state_ == AgentState::Return) {
		desired = Normalize(toCenter);
	}

	Vector3 steer = Normalize(desired + sep * 0.5f + cohesion * 0.8f);
	float targetSpeed = speed;

	switch (state_) {
	case AgentState::Idle:              targetSpeed = 0.0f; break;
	case AgentState::PreparationAttack: targetSpeed *= 0.3f; break;
	case AgentState::Attack:            targetSpeed *= 0.1f; break;
	case AgentState::Return:            targetSpeed *= 0.8f; break; // ★素早く戻る
	default: break;
	}

	velocity_ = Lerp(velocity_, steer * targetSpeed, dt * 5.0f);
	//velocity_.y = 0;
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
		// 部隊目標へ向かうためにスロット位置を少し前方へオフセット
		Vector3 dirToCmd = Normalize(command.targetPos - centerPos);
		Vector3 finalTarget = { slot.x + dirToCmd.x * 0.5f, slot.y, slot.z + dirToCmd.z * 0.5f };
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
		// 距離に応じたLOD: 遠い奴は更新間引き（簡略化）
		float d = Length(a.position_ - playerPos);
		float lodFactor = (d < 15.0f) ? 1.0f : ((d < 40.0f) ? 0.5f : 0.25f);
		// シンプルに更新をスキップする代わりに時間経過での補間は省略
		if (Random::RandomFloat(0.0f, 1.0f) > lodFactor) continue;


		grid.QueryNeighbors(a.position_, neighbors);
		// 部隊目標 + スロットオフセットを計算
		CrowdGroupAgent& grp = groups[a.groupId];
		// ローカルインデックスを求める（簡略な実装）
		int localIndex = -1;
		for (size_t k = 0; k < grp.memberIndices.size(); ++k) {
			if (grp.memberIndices[k] == (int)i) {
				localIndex = (int)k;
				break;
			}
		}
		Vector3 slotTarget = (localIndex >= 0) ? grp.CalcSlotTarget(localIndex) : grp.centerPos;
		// 最終目標は部隊コマンドターゲットに向かうためのslotTarget
		Vector3 finalTarget = { slotTarget.x + (grp.command.targetPos.x - grp.centerPos.x) * 0.5f,
		slotTarget.y,
		slotTarget.z + (grp.command.targetPos.z - grp.centerPos.z) * 0.5f };


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
