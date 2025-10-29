#pragma once
#include <map>
#include"DirectXGame/engine/3d/Object/Object3dInstansManager.h"



// 前方宣言
class BaseCharacter;


/// <summary>
/// エージェントの状態
/// </summary>
enum class AgentState {
	Idle,			// 待機 ->Approach
	Approach,		// 接近 ->Attack
	Attack,			// 攻撃 ->Evade or Idle
	Evade,			// 回避 ->
	Dead			// 死亡 ->
};


// 前方宣言
class CrowdGroupAgent;


/// <summary>
/// 個々のキャラクター
/// </summary>
class CrowdAgent {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(uint32_t _id, uint32_t _group, const Vector3& pos);

	/// <summary>
	/// GroupCommandから受け取ったターゲットへ向かう処理（分散 + 衝突回避を含む）
	/// </summary>
	void Update(float dt, const Vector3& groupTarget, const std::vector<int>& neighborIndices, const std::vector<CrowdAgent>* allAgents);

	/// <summary>
	/// 所属している部隊登録
	/// </summary>
	void SetGroupAgent(CrowdGroupAgent* group) { group_ = group; }






	Vector3 ComputeSteering(const Vector3& target) const
	{
		Vector3 pos = position_;
		Vector3 tgt = target;
		Vector3 dir = Normalize(tgt - pos);
		return dir;
	}

	AgentState state_ = AgentState::Idle;	// 状態
	Vector3 position_ = {};					// 位置
	Vector3 velocity_ = {};					// 速度
	float attackCooldown_ = 0.0f;			// 攻撃クールタイム
	uint32_t id;							// 個人ID
	uint32_t groupId;                       // 部隊ID
	float speed = 3.0f; // m/s
	float radius = 0.5f; // 衝突半径

	Vector3 targetPos_ = {};				// ターゲット位置

	// 表示用：インスタンスバッファに書き込むデータ
	uint32_t animIndex = 0;
private:
	CrowdGroupAgent* group_ = nullptr;
};


enum class GroupAction : uint8_t { Idle, Advance, Attack, Retreat };


struct GroupCommand
{
	GroupAction action = GroupAction::Idle;
	Vector3 targetPos = { 0,0,0 };
	float moveSpeed = 2.5f;
};

/// <summary>
/// 部隊
/// </summary>
class CrowdGroupAgent {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="name"></param>
	void Init(std::string name);


	/// <summary>
	/// ターゲット取得
	/// </summary>
	void SetTarget(BaseCharacter* character);


private:

	void UpdateCenter(const std::vector<CrowdAgent>& agents)
	{
		if (memberIndices.empty()) return;
		Vector3 sum = { 0,0,0 };
		for (int idx : memberIndices)
		{
			sum = sum + agents[idx].position_;
		}
		centerPos = sum * (1.0f / (float)memberIndices.size());
	}

	// 各個体のフォーメーション上の目標位置を計算して返す
	Vector3 CalcSlotTarget(int localIndex)
	{
		int col = localIndex % formationCols;
		int row = localIndex / formationCols;
		float x = (col - formationCols / 2) * formationSpacing;
		float z = (float)row * formationSpacing;
		// 部隊の向きを考慮して回転させる場合はここで行う（簡略化のため正面は+Z）
		return { centerPos.x + x, centerPos.y, centerPos.z + z };
	}

	// 部隊のコマンドに応じて内部ロジックでtargetPosを調整
	void UpdateCommandLogic(const Vector3& playerPos)
	{
		float d = Length(playerPos - centerPos);
		if (d < 12.0f) { command.action = GroupAction::Attack; command.targetPos = playerPos; command.moveSpeed = 3.0f; }
		else if (d < 30.0f) { command.action = GroupAction::Advance; command.targetPos = playerPos; command.moveSpeed = 2.2f; }
		else { command.action = GroupAction::Idle; command.targetPos = centerPos; command.moveSpeed = 0.0f; }
	}


	// メンバーに与える目標（部隊目標 + 各スロット）を作る
	void DistributeTargets(std::vector<CrowdAgent>& agents)
	{
		for (size_t i = 0; i < memberIndices.size(); ++i)
		{
			int idx = memberIndices[i];
			if (idx < 0) continue;
			Vector3 slot = CalcSlotTarget((int)i);
			// 部隊目標へ向かうためにスロット位置を少し前方へオフセット
			Vector3 dirToCmd = Normalize(command.targetPos - centerPos);
			Vector3 finalTarget = { slot.x + dirToCmd.x * 0.5f, slot.y, slot.z + dirToCmd.z * 0.5f };
			// Agentが持つ追加パラメータを通して使う（ここでは直接更新しても良い）
			agents[idx].state_ = AgentState::Approach; // まずはMoveにする
			// ここでは簡単のため agents[idx] に target 情報を渡さない。CrowdManager側で更新時に group.command.targetPos と slot を参照する。
		}
	}
	// 部隊名前
	std::string name_ = "";
	// 部隊人数
	uint32_t number_ = 0;
	
	uint32_t id;
	std::vector<int> memberIndices; // CrowdManager側のエージェント配列へのインデックス
	Vector3 centerPos = { 0,0,0 };
	GroupCommand command;


	// simple formation: grid parameters
	int formationCols = 8;
	float formationSpacing = 1.2f;

private:
	// ターゲットのキャラクター
	BaseCharacter* character_ = nullptr;
};




/// <summary>
/// 群衆LOD
/// </summary>
/// プレイヤーの位置によって部隊達がどう行動レベル、描画レベルを管理するクラス
class CrowdLOD {
public:





private:
	int leve_ = 0;					// レベル
	std::vector<bool> conditions_;	// 条件
};


/// <summary>
/// 空間分割
/// </summary>
struct UniformGrid
{
	float cellSize = 4.0f;
	int cols = 64;
	int rows = 64;
	float originX = -128.0f;
	float originZ = -128.0f;
	std::vector<std::vector<int>> cells; // 各セルのagentインデックス


	UniformGrid(int c = 64, int r = 64, float cs = 4.0f) : cellSize(cs), cols(c), rows(r)
	{
		cells.resize(cols * rows);
	}


	void Clear() { for (auto& v : cells) v.clear(); }


	int PosToCellIndex(const Vector3& p) const
	{
		int cx = (int)floorf((p.x - originX) / cellSize);
		int cz = (int)floorf((p.z - originZ) / cellSize);
		if (cx < 0 || cz < 0 || cx >= cols || cz >= rows) return -1;
		return cz * cols + cx;
	}


	void Insert(int agentIndex, const Vector3& pos)
	{
		int idx = PosToCellIndex(pos);
		if (idx >= 0) cells[idx].push_back(agentIndex);
	}


	// 簡易近傍取得：同セルと周辺8セルを走査して、結果インデックスを返す
	void QueryNeighbors(const Vector3& pos, std::vector<int>& out)
	{
		out.clear();
		int cx = (int)floorf((pos.x - originX) / cellSize);
		int cz = (int)floorf((pos.z - originZ) / cellSize);
		for (int dz = -1; dz <= 1; ++dz) for (int dx = -1; dx <= 1; ++dx)
		{
			int nx = cx + dx; int nz = cz + dz;
			if (nx < 0 || nz < 0 || nx >= cols || nz >= rows) continue;
			int idx = nz * cols + nx;
			auto& cell = cells[idx];
			out.insert(out.end(), cell.begin(), cell.end());
		}
	}
};



/// <summary>
/// 群衆AI管理
/// </summary>
class CrowdManager {
public:
	UniformGrid grid;
	/// <summary>
	/// 部隊コンテナ
	/// </summary>
	std::vector<CrowdAgent> agents;
	std::vector<CrowdGroupAgent> groups;
	
	// 
	CrowdLOD lod_;

	//
	UniformGrid grid_;


};