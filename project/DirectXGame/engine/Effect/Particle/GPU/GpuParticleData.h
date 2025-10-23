#pragma once
#include "DirectXGame/engine/Math/MathFanctions.h"

#include "DirectXGame/engine/DirectX/Resource/StructuredBuffer.h"
#include "DirectXGame/engine/DirectX/Resource/ConstantBuffer.h"



struct ParticleCS {
	Vector4 color;

	Vector3 transrate;
	float lifeTime;

	Vector3 scale;
	float currentTime;

	Vector3 velocity;
	uint32_t isAlhpa;

	Vector3 acceleration;
	uint32_t isScaling;

	float scaleAmount;
	Vector3 prevTranslate; 

	Vector4 trailcolor;     // リボンの色

	uint32_t isGravity;// 重力影響するか
	uint32_t isTrail;  // リボンを引くか
	float width;       // リボンの太さ
	float trailLifeTime;    // トレイル生存時間

	uint32_t trailHeadIndex;       // リングバッファの先頭インデックス
	Vector3 trailPrevEnd;       // 前フレームの末端位置

	uint32_t hasPrevQuad;	// 前のQuadが有効か
	Vector3 rotation;				// 回転

	uint32_t isAlive;           // 生存しているか
	uint32_t useBillboard;      // ビルボードを使うか
	uint32_t emitterIndex;
	uint32_t pad;
};

// パーティクル削除
struct DeleteParticleCS {
	uint32_t isDelete;
	Vector3 pad;
};

struct PreView {
	Matrix4x4 viewProjection;
	Matrix4x4 billboardMatrix;
	Matrix4x4 billboardMatrixY;

};

struct PerFrame
{
	// ゲームを起動してからの時間
	float time;
	// 1フレームの経過時間
	float deltaTime;
};



// 出方
enum class ParticleSpawnShape : uint32_t
{
	Volume = 0, // 形状内ランダム
	Surface,    // 面上
	Edge        // 辺上
};

// 方向
enum class ParticleDireccion : uint32_t
{
	Random = 0,     // ランダム
	Outward,        // 外向き
	Inward,			// 内向き
};

// エミッタータイプ
enum class EmitterType : uint32_t
{
	Sphere = 0,     // 球体
	AABB,           // AABB
	Point           // 点
};

// 補間方法
enum class EmitterInterpolation : uint32_t
{
	NoUse = 0,			// 補間しない
	Random = 1,			// ランダム
	Sequential = 2,     // 順番
};

// 共通エミッター
struct EmitterCommon
{
	/// 共通部分

	Vector3 scale;						// サイズ
	float lifeTime;						// 生存時間
	
	Vector3 scaleRange;					// サイズ(範囲)
	float lifeTimeRange;				// 生存時間(範囲)
	
	Vector3 velocity;					// 速度
	uint32_t count;						// 射出数
	
	Vector3 velocityRange;				// 速度(範囲)
	uint32_t emit;						// 射出許可
	
	Vector3 color;						// 色
	float frequency;					// 射出間隔
	
	Vector3 colorRange;					// 色(範囲)
	float frequencyTime;				// 射出間隔調整用時間
	
	Vector3 translate;					// 位置
	ParticleSpawnShape spawnShape;		// 出方(0:形状内ランダム, 1:面上, 2:辺上)
	
	ParticleDireccion directionType;	// 方向(0:ランダム, 1:外向き, 2:内向き)
	float force;						// 力 
	uint32_t isAlhpa;					// アルファブレンドするか
	uint32_t isScaling;
	
	Vector3 rotate;						// 回転
	uint32_t isGravity;					// 重力影響するか

	Vector3 rotateRange;				// 回転(範囲)
	uint32_t useBillboard;				// ビルボードを使うか
	
	float scaleAmount;					// スケーリング量
	Vector3 prevTranslate;				// 前フレームの位置


	///
	EmitterType shapeType;				// 形状


	/// <summary>
	/// 球
	/// </summary>
	float sphereRadius;					// 射出半径(球)
	
	
	/// <summary>
	/// AABB
	/// </summary>
	Vector3 size;						// 射出サイズ(AABB)

	/// <summary>
	///  点
	/// </summary>
	EmitterInterpolation interpolation; // 補間方法(0:しない,1:ランダムに,2:順番)

	// ===== 🔹 新規追加（パーティクル範囲） =====
	uint32_t particleStartOffset;   // GPUパーティクルバッファ上の開始インデックス
	uint32_t particleMaxCount;      // このEmitterに割り当てられた最大粒子数

};




// エミッターリボン
struct EmitterTrail
{
	Vector4 trailcolor;     // リボンの色
	
	uint32_t isTrail;       // リボンを引くか
	float trailWidth;       // リボンの太さ
	float trailLifeTime;	// リボンの生存時間
	float pad;
};


// フィールド
struct EffectFieldCS {
	Vector3 translate;      // 位置
	float force;			// 力
	Vector3 range;			// 各半径
	uint32_t isEffect;		// 影響を出すか
};


// 最大個数
struct MaxInstance
{
	uint32_t maxInstance;	// 最大個数
};
// 空きリストのインデックス
struct FreeListIndex {
	int32_t index;
};
// 生存数
struct ParticleCount
{
	int32_t count;
};

// 
struct PerEmitterDispatch {
	uint32_t startThread;       // このエミッタが処理を始めるスレッドのインデックス
	uint32_t totalThreadCount;  // このエミッタが必要とするスレッド数
	uint32_t particleOffset;    // 書き込み先パーティクルバッファの開始インデックス
	uint32_t emitterIndex;      // エミッタ番号（StructuredBufferのインデックス）
};


struct DispatchCount {
	uint32_t gEmitterDispatchCount;
	uint32_t pad0;
	uint32_t pad1;
	uint32_t pad2;
};

// 頂点データ(トレイル)
struct GpuTrailVertex
{
	Vector3 position;	// 座標
	Vector2 uv;			// uv
	Vector3 normal;		// 法線
	Vector4 color;		// 色
	float lifeTime;		// 生存時間
	float currentTime;	// 経過時間
	uint32_t isAlive;	// 生存しているか
	float pad;
};

