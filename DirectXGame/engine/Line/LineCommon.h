#pragma once
#include <array>
#include <cstdint>
#include <d3d12.h>
#include <memory>
#include <string>
#include <wrl.h>
#include "DirectXGame/engine/struct/Structs.h"
#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/base/DirectXCommon.h"
#include "DirectXGame/engine/Mesh/Mesh.h"


class LineCommon
{
public:

public:
	static LineCommon* GetInstance();

	// 終了
	void Finalize();

	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	//void Update

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	void AddLine(Vector3 pos ,Vector3 pos2);

	void AddLineMesh(Mesh* mesh, const Matrix4x4& worldMat);
	void AddLineMesh(Mesh* mesh, const Matrix4x4& worldMat, std::vector<uint32_t> cachedLineIndices);
	
	void Update();

	void DrawCommonSetting();

	void Draw();

	void SetDefaltCamera(Camera* camera) { camera_ = camera; }


	void LineClear();

	struct LineGPU {
		/*Matrix4x4 WVP;
		Matrix4x4 World;
		*/
		Vector4 color;
		Vector3 strPos;
		float pad;
		Vector3 endPos;
		float pad2;
	};
private:
	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();


private:
	static LineCommon* instance;

	Camera* camera_ = nullptr;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;

	DirectXCommon* dxCommon_;
	std::unique_ptr<Mesh> mesh_;
	//std::unique_ptr<Mate>
	const uint32_t kNumMaxInstance = 100000;
	
	//マテリアルデータ
	struct MaterialData {
		Vector4 color;
	};
	Microsoft::WRL::ComPtr < ID3D12Resource> materialResource;
	MaterialData* materialData;

	//Microsoft::WRL::ComPtr<ID3D12Resource> resource;
	//uint32_t instanceCount; // インスタンス数
	//LineGPU* instanceData; // インスタンシングデータを書き込むためのポインタ
	//std::vector<LineGPU> line_;
	//D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU;
	//D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU;
	//uint32_t srvIndex;

	Microsoft::WRL::ComPtr<ID3D12Resource> viewResource;
	Matrix4x4* cameraWVP;


	uint32_t lineNum_ = 0;
};

