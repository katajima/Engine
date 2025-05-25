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
#include "DirectXGame/engine/DirectX/Resource/ConstantBuffer.h"

#include<random>
#include<numbers>



class OceanManager;
class Entity3DManager;
class DirectXCommon;
class Ocean
{
private:
	// ウェーブデータ
	struct WaveParameters {
		Vector2 waveDirection; // 波の方向
		float amplitude; // 波の振幅
		float frequency; // 波の周波数
		float speed; // 波の速度
		float time; // 現在の時間（アニメーション用）
		int flag;   // 波が起きるのか
		float pad[1];
	};
public:
	

	// 大きさ
	void Initialize(Entity3DManager* entity3dManager ,Vector2 renge);

	void Update();

	void UpdateImgui();

	void Draw();

	Material* GetMaterial() { return material.get(); }

	ModelMesh* GetMesh() { return mesh_.get(); }


	WaveParameters* GetWaveParameters() const { return cbWaveResource_.Data(); }

private:


	void AddWave();

private:
	// マテリアル
	std::unique_ptr<Material> material = nullptr;
	// メッシュ
	std::unique_ptr<ModelMesh> mesh_ = nullptr;
private:
	

	ConstantBuffer<WaveParameters> cbWaveResource_;

	// ノイズデータ
	struct NoiseData {
		float noiseScale; // ノイズのスケール
		float noiseStrength; // ノイズの強度
		int octaves; // フラクタルノイズのオクターブ数
		float roughness; // 各オクターブの影響度
	};

	ConstantBuffer<NoiseData> cbNoiseResource_;


	// 幅	
	struct Renge {
		Vector2 renge;
	};
	Renge renge_;

	// 波の合成数
	uint32_t index_ = 0;

	
	Entity3DManager* entity3dManager_;
	DirectXCommon* directXCommon_;
};
