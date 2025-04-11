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
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/struct/Material.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"

#include"DirectXGame/engine/Manager/SRV/SrvManager.h"

#include "DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3dCommon.h"
#include "DirectXGame/engine/3d/Model/Model.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/Primitive/Primitive.h"
#include"DirectXGame/engine/PSO/PSOManager.h"

#include <thread>
#include <mutex>


class ObjectInstans
{
public:
	// 初期化
	void Initialize(Transform transform = { {1,1,1},{},{} });
	// 更新
	void Update();


private:

public:
	WorldTransform transform;
	Vector4 color;
	bool is;
	uint32_t texIndex;


};

class Object3dInstansManager
{
public:
	struct ObjectGPU
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
		UINT textureIndex;
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

	enum class MeshType
	{
		kPrimitiv,
		kModel,
	};

	struct Object
	{
		WorldTransform transform;
		Vector4 color;
		bool is;
		uint32_t texIndex;

	};

	struct ObjectGroup
	{
		std::string name; // 名前
		std::vector<ObjectInstans> object;
		Model* model;
		bool flag;
		uint32_t srvIndex;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		uint32_t instanceCount; // インスタンス数
		ObjectGPU* instanceData; // インスタンシングデータを書き込むためのポインタ
		D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;
		D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU;
		Mesh* mesh;
		RasterizerType rasteType;
		BlendType blendType;
	};

public:
	// シングルトンインスタンス
	static Object3dInstansManager* GetInstance();

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

	// パーティクルグループ作り(モデル)
	void CreateObject3dGroup(const std::string name, const std::string textureFilePath, Model* model, RasterizerType rasteType = RasterizerType::MODE_SOLID_BACK, BlendType blendType = BlendType::MODE_ADD);
	// パーティクルグループ作り(モデル)
	void CreateObject3dGroup(const std::string name, const std::string textureFilePath, Mesh* mesh, RasterizerType rasteType = RasterizerType::MODE_SOLID_BACK, BlendType blendType = BlendType::MODE_ADD);

	// カメラセット
	void SetCamera(Camera* camera) { this->camera_ = camera; }

	void AddObject(const std::string name, const std::string texName, ObjectInstans& object, MeshType type = MeshType::kModel);

	int GetSize() { return static_cast<int>(objectGroups.size()); };

	void Clear(const std::string name);

private:
	
	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();

	void BlendAdd();

	void BlendSubtract();

	void BlendMuliply();


	static Object3dInstansManager* instance;
	Object3dInstansManager() = default;
	~Object3dInstansManager() = default;
	Object3dInstansManager(Object3dInstansManager&) = delete;
	Object3dInstansManager& operator=(Object3dInstansManager&) = delete;

	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;



	std::unique_ptr<PSOManager> psoManager_ = nullptr;
	ModelManager* modelManager_;
	

	std::unordered_map<std::string, ObjectGroup> objectGroups;


	const uint32_t kNumMaxInstance = 100000;

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









