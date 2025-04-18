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
using namespace Microsoft::WRL;
#include<vector>
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/struct/Material.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"

#include"DirectXGame/engine/Manager/SRV/SrvManager.h"
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/Light/LightCommon.h"
#include"DirectXGame/engine/Transfomation/Transfomation.h"
#include"DirectXGame/engine/Material/Material.h"

#include<random>
#include<numbers>



class OceanManager;
class Entity3DManager;
class DirectXCommon;
class Ocean
{
public:
	

	// 大きさ
	void Initialize(Entity3DManager* entity3dManager ,Vector2 renge);

	void Update();

	void Draw();

	void SetCamera(Camera* camera) { this->camera = camera; }


public:
	// マテリアル
	std::unique_ptr<Material> material = nullptr;
	// メッシュ
	std::unique_ptr<Mesh> mesh_ = nullptr;

	// マトリックス
	Matrix4x4 mat_;
	// 位置
	Transform transform;


private:
	struct WaveParameters {
		Vector2 waveDirection; // 波の方向
		float amplitude; // 波の振幅
		float frequency; // 波の周波数
		float speed; // 波の速度
		float time; // 現在の時間（アニメーション用）
		int flag;   // 波が起きるのか
		float pad[1];
	};
	Microsoft::WRL::ComPtr < ID3D12Resource> waveResource;
	WaveParameters *waveData;

	struct NoiseData {
		float noiseScale; // ノイズのスケール
		float noiseStrength; // ノイズの強度
		int octaves; // フラクタルノイズのオクターブ数
		float roughness; // 各オクターブの影響度
	};
	Microsoft::WRL::ComPtr < ID3D12Resource> noiseResource;
	NoiseData* noiseData;


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
		std::vector<uint32_t> indices; // 追加：インデックスデータ
		std::vector<VertexData> indicesPos;
		MaterialData material;
	};
	
	struct Renge {
		Vector2 renge;
	};
	Renge renge_;

	
	std::unique_ptr<Transfomation>transfomation = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr < ID3D12Resource> vertexResource;
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;

	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;


	Microsoft::WRL::ComPtr < ID3D12Resource> indexResource;
	uint32_t* indexData = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;


	Microsoft::WRL::ComPtr < ID3D12Resource> materialResource;

	
	Camera* camera = nullptr;
	Entity3DManager* entity3dManager_;
	DirectXCommon* directXCommon_;
};
