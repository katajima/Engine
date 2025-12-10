#pragma once
#include <map>
#include"DirectXGame/engine/3d/Object/Object3dInstansManager.h"



// 前方宣言
class BaseCharacter;


/// <summary>
/// エージェントの状態
/// </summary>
enum class AgentState {
	Idle,				// 待機 ->Approach
	Approach,			// 接近 ->Attack
	PreparationAttack,	// 攻撃準備
	Attack,				// 攻撃 ->Evade or Idle
	Return,				// 戻る ->
};


// 前方宣言
class CrowdGroupAgent;
class BaseEnemy;
class CrowdManager;

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

	/// <summary>
	/// 管理クラス設定
	/// </summary>
	/// <param name="manager"></param>
	void SetCrowdManager(CrowdManager* manager) { manager_ = manager; }


	Vector3 ComputeSteering(const Vector3& target) const
	{
		Vector3 pos = position_;
		Vector3 tgt = target;
		Vector3 dir = Normalize(tgt - pos);
		return dir;
	}

	// 敵
	BaseEnemy* owner_ = nullptr;


	AgentState state_ = AgentState::Idle;	// 状態
	Vector3 position_ = {};					// 位置
	Vector3 velocity_ = {};					// 速度
	float attackCooldown_ = 0.0f;			// 攻撃クールタイム
	uint32_t id;							// 個人ID
	uint32_t groupId;                       // 部隊ID
	float speed = 30.0f;					// m/s
	float radius = 0.5f;					// 衝突半径

	Vector3 targetPos_ = {};				// ターゲット位置

	bool isDed = false;						// 死んでいるか

	float attackDelayTimer_ = 0.0f;			// 攻撃前の準備タイマー
	float engageDistance_ = 200.0f;			// 攻撃を検討し始める距離
	float attackRange_ = 7.5f;				// 実際に攻撃できる距離

	// 表示用：インスタンスバッファに書き込むデータ
	uint32_t animIndex = 0;
	Vector3 slotTarget_;
private:
	
	CrowdManager* manager_ = nullptr;
	CrowdGroupAgent* group_ = nullptr;
};


enum class GroupAction : uint8_t { 
	Idle, 
	Advance, 
	Attack, 
	Retreat 
};


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
	

	void Initialize(const Vector3& startPos)
	{
		centerPos = startPos;
		anchorCenter = startPos; // 理想中心
	}

	// グループの中心位置
	void UpdateCenter(const std::vector<CrowdAgent>& agents);
	
	// 各個体のフォーメーション上の目標位置を計算して返す
	Vector3 CalcSlotTarget(int localIndex);
	
	// 部隊のコマンドに応じて内部ロジックでtargetPosを調整
	void UpdateCommandLogic(const Vector3& playerPos);
	
	// メンバーに与える目標（部隊目標 + 各スロット）を作る
	void DistributeTargets(std::vector<CrowdAgent>& agents);
	



	// 部隊人数
	uint32_t number_ = 0;
	uint32_t id;
	std::vector<int> memberIndices; // CrowdManager側のエージェント配列へのインデックス
	Vector3 centerPos = { 0,0,0 };
	Vector3 anchorCenter = { 0,0,0 };
	GroupCommand command;
	// simple formation: grid parameters
	int formationCols = 8;
	float formationSpacing = 1.2f;

private:
	// ターゲットのキャラクター
	BaseCharacter* character_ = nullptr;
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
	
	Vector3 playerPos = { 0,0,0 };


	CrowdManager(int maxAgents = 1024, int maxGroups = 32) : grid(64, 64, 4.0f)
	{
		agents.reserve(maxAgents);
		groups.reserve(maxGroups);
	}


	int CreateGroup()
	{
		CrowdGroupAgent g; g.id = (uint32_t)groups.size();
		groups.push_back(std::move(g));
		return (int)groups.size() - 1;
	}

	CrowdGroupAgent& GetGroup(uint32_t id) { return groups[id]; }

	int CreateAgent(int groupId, BaseEnemy* ene,const Vector3 pos);

	// LODコントロールの簡易実装: 距離に応じてAI更新の頻度や詳細度を落とす
	void Update(float dt);
	

	void BindAgentsToEnemies(std::vector<BaseEnemy*>& enemies);

	void UpdateAgentsToInstancing();
};