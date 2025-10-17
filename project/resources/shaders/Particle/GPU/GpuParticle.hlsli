

// パーティクル
struct Particle
{
    float4 color;
    
    float3 translate;
    float lifeTime;
    
    float3 scale;
    float currentTime;
    
    float3 velocity;
    uint isAlpha;
    
    float3 acceleration;
    uint isScaling;
    
    float scaleAmount;
    float3 prevTranslate;
    
    float4 trailColor;      // リボンの色

    uint isGravity;         // 重力影響するか
    uint isTrail;           // リボンを引くか
    float trailWidth;       // リボンの太さ
    float trailLifeTime;    // トレイル生存時間
    
    uint trailHeadIndex;    // リングバッファの先頭インデックス
    float3 trailPrevEnd;    // 前フレームの末端位置
    
    uint hasPrevQuad;       // 前のQuadが有効か
    float3 rotation;        // 回転
    
    
    uint isAlive;           // 生存しているか
    uint useBillboard;      // ビルボードを使うか
    float2 pad;
    
    
    
};

struct DeleteParticle
{
    uint isDelete; // 削除
    float3 pad;
};

struct PerView
{
    float4x4 viewProjection;
    float4x4 billboardMatrix;
    float4x4 billboardMatrixY;
};





// エミッター共通
struct EmitterCommon
{
    float3 scale;           // サイズ
    float lifeTime;         // 生存時間
    
    float3 scaleRange;      // サイズ(範囲)
    float lifeTimeRange;    // 生存時間(範囲)
    
    float3 velocity;        // 速度
    uint count;             // 射出数
    
    float3 velocityRange;   // 速度(範囲)
    uint emit;              // 射出許可
    
    float3 color;           // 色
    float frequency;        // 射出間隔
    
    float3 colorRange;      // 色(範囲)
    float frequencyTime;    // 射出間隔調整用時間
    
    float3 translate;       // 位置
    uint spawnShape;        // 出方(0:形状内ランダム, 1:面上, 2:辺上)
    
    uint directionType;     // 方向(0:ランダム, 1:外向き, 2:内向き)
    float force;            // 力 
    uint isAlpha;           // アルファブレンドするか
    uint isScaling;         // スケーリングするか
    
    float3 rotate;          // 回転
    uint isGravity;         // 重力影響するか
    
    float3 rotateRange;     // 回転(範囲)
    uint useBillboard;      // ビルボードを使うか
    
    
    float scaleAmount;      // スケーリング量
    float3 prevTranslate;   // 前フレームの位置
    
    
    
    ///
    uint shapeType; // 形状


	/// <summary>
	/// 球
	/// </summary>
    float sphereRadius; // 射出半径(球)
	
	
	/// <summary>
	/// AABB
	/// </summary>
    float3 size; // 射出サイズ(AABB)

	/// <summary>
	///  点
	/// </summary>
    uint interpolation; // 補間方法(0:しない,1:ランダムに,2:順番)
    
    uint particleStartOffset; // GPUパーティクルバッファ上の開始インデックス
    uint particleMaxCount; // このEmitterに割り当てられた最大粒子数
};



// トレイル関係エミッター
struct EmitterTrail
{
    float4 trailColor; // リボンの色
    
    uint isTrail; // リボンを引くか
    float trailWidth; // リボンの太さ
    float trailLifeTime; // リボンの生存時間 
    float pad; // パディング
};

// エミッター(球)
struct EmitterSphere
{
    float radius; // 射出半径
};
// エミッター(AABB)
struct EmitterAABB
{
    float3 size; // 大きさ
};
// エミッター(点)
struct EmitterPoint
{
    uint interpolation;
    float3 pad; // パディング
};


struct PerFrame
{
    // ゲームを起動してからの時間
    float time;
    // 1フレームの経過時間
    float deltaTime;
};

// 最大個数
struct MaxInstance
{
    uint maxInstanse;
};



// Field影響
struct EffectFieldCS
{
    float3 translate; // 位置
    float force; // 力
    float3 range; // 各半径
    uint isEffect; // 影響を出すか
};


// リボン用
struct RibbonVertex
{
    float3 pos;         // 座標
    float2 uv;          // UV
    float3 normal;      // 法線
    float4 color;       // 色
    float lifeTime;     // 生存時間
    float currentTime;  // 経過時間
    uint isAlive;       // 生存しているか
    float pad;
};


struct CameraPos
{
    float3 cameraPos; // カメラ位置
};

// リボンの頂点使用数
struct GpuTrailCount
{
    uint vertexcount; // 使用数(Vertex)
    uint vertexMaxCount; // 最大数(Vertex)
    uint indexCount; // 使用数(Index)
    uint indexMaxCount; // 最大数(Index)
};

struct PerEmitterDispatch
{
    uint startThread; // このエミッタが処理を始めるスレッドのインデックス
    uint totalThreadCount; // このエミッタが必要とするスレッド数
    uint particleOffset; // 書き込み先パーティクルバッファの開始インデックス
    uint emitterIndex; // エミッタ番号（StructuredBufferのインデックス）
};