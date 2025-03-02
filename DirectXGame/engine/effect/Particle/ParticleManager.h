#pragma once
#include <windows.h>
#include<cstdint>
#include<string>
#include<fstream>
#include<sstream>
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
#include<memory>
using namespace Microsoft::WRL;
#include<vector>
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/struct/Structs.h"
#include"DirectXGame/engine/struct/Material.h"
#include"DirectXGame/engine/base/DirectXCommon.h"
#include"DirectXGame/engine/base/SrvManager.h"
#include<random>
#include<numbers>
#include "DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object3dCommon.h"
#include "DirectXGame/engine/3d/Model.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/Primitive/Primitive.h"
#include "../../3d/Object3d.h"
#include"DirectXGame/engine/PSO/PSOManager.h"


struct ParticleForGPU
{
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};
// 頂点データ
struct VertexData {

	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};


//モデルデータ
struct ModelData
{
	std::vector<VertexData> vertices;
	MaterialData material;
	Node rootNode;
};




class ParticleManager
{
public:


	// 
	enum class SpawnType // 出現形状
	{
		kAABB,		// AABB
		kOBB,		// OBB
		kSphere,	// Sphere



		kSegmentLine,		// Line
		kCornerLine,        // コーナーライン
		kSpline,			// スプライン
	};
	enum class EmitType
	{
		kRandom,   // ランダム
	};
	enum class RasterizerType
	{
		MODE_SOLID_BACK,
		MODE_SOLID_NONE,
	};

	enum class BlendType
	{
		MODE_ADD,
		MODE_SUBTRACT,
		MODE_MUlLIPLY,
	};
#pragma region structs
	template<typename T>
	struct MaxMin
	{
		T min;
		T max;
	};



	// エミッター構造体
	struct Emiter
	{
		// ランダム用
		MaxMin<Vector3> renge;     //出現位置 (Vector3の範囲)
		MaxMin<Vector4> color;     // 色 (Vector3の範囲)
		MaxMin<Vector3> size;        // 大きさ (floatの範囲)
		MaxMin<Vector3> rotate;      // 回転 (floatの範囲)
		MaxMin<Vector3> rotateVelocity;// 回転 (floatの範囲)
		MaxMin<float> lifeTime;    // 生存時間 (floatの範囲)
		MaxMin<Vector3> velocity;  // 速度 (Vector3の範囲)

		CornerSegment corner;
		std::vector<Vector3> controlPoints; // 各ポジション

		WorldTransform worldtransform;
		bool isEmit = false;



		int count;
	};

	struct Particle
	{
		Transform transform;
		Vector3 velocity;
		Vector3 acceleration;
		Vector4 color;
		float lifeTime;
		float currentTime;

		Transform strtTransform;
		Vector3 rotateVelocity;
	};

	struct ParticleGroup
	{
		std::string name; // 名前
		std::unique_ptr<Material> material = nullptr;
		std::list<Particle> particle;
		bool flag;
		uint32_t srvIndex;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		uint32_t instanceCount; // インスタンス数
		ParticleForGPU* instanceData; // インスタンシングデータを書き込むためのポインタ
		D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;
		D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU;
		Mesh* mesh;
		Emiter emiter;
		bool usebillboard = true;
		bool isAlpha = false;
		bool isLine = true;
		bool isGravity = false;
		bool isLifeTimeScale_ = false;
		bool isRotateVelocity = false;
		bool isBounce = false;
		EmitType emitType = EmitType::kRandom;
		RasterizerType rasteType;
		BlendType blendType;
	};



#pragma endregion // 構造体

public:


	// シングルトンインスタンス
	static ParticleManager* GetInstance();

	// 初期化
	void Initialize(DirectXCommon* dxCommon);
	// 更新
	void Update();
	// 描画
	void Draw();
	// 終了
	void Finalize();
	// 描画準備
	void DrawCommonSetting(RasterizerType rasteType, BlendType blendType);

	// パーティクルの発生
	void Emit(const std::string name, EmitType type, SpawnType spawnType);

	// パーティクルグループ取得
	std::unordered_map<std::string, ParticleGroup>& GetParticleGroups()
	{
		return particleGroups;
	}
	// パーティクルグループ取得
	ParticleGroup& GetParticleGroups(const std::string name)
	{
		return particleGroups[name];
	}


	// パーティクルグループ作り(モデル)
	void CreateParticleGroup(const std::string name, const std::string textureFilePath, Model* model, RasterizerType rasteType = RasterizerType::MODE_SOLID_BACK, BlendType blendType = BlendType::MODE_ADD);
	// パーティクルグループ作り(プリミティブ)
	void CreateParticleGroup(const std::string name, const std::string textureFilePath, Primitive* primitive, RasterizerType rasteType = RasterizerType::MODE_SOLID_BACK, BlendType blendType = BlendType::MODE_ADD);

	// カメラセット
	void SetCamera(Camera* camera) { this->camera_ = camera; }

private:
	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();

	// minmax
	void LimitMaxMin();

	// ランダム
	void RandParticle(const std::string name, SpawnType spawnType);

	

	void BlendAdd();

	void BlendSubtract();

	void BlendMuliply();

private: // エミッタ種類
	void AABBEmit(ParticleGroup& particleGroup);

	void LineEmit(ParticleGroup& particleGroup);

	void CornerLineEmit(ParticleGroup& particleGroup);

	void SplineEmit(ParticleGroup& particleGroup);


private:
	static ParticleManager* instance;
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(ParticleManager&) = delete;
	ParticleManager& operator=(ParticleManager&) = delete;

	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;



	std::unique_ptr<PSOManager> psoManager_ = nullptr;

	std::mt19937 randomEngine_;


	std::unordered_map<std::string, ParticleGroup> particleGroups;


	const uint32_t kNumMaxInstance = 12000;
	//float kDeltaTime;
	bool usebillboard = true;
	bool upData = true;
	bool upDataWind = false;
	uint32_t numInstance{};


	Camera* camera_ = nullptr;


	Transform transform{};


	const float kGravitationalAcceleration = 9.8f;
	////ルートシグネチャデスク
	D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
	////ルートシグネチャ
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
	//// グラフィックスパイプラインステート
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState[6];



	D3D12_BLEND_DESC blendDesc{};
	D3D12_RASTERIZER_DESC rasterizerDesc{};
};



