#pragma once
// DirectX
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>

// DirectXTex
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"

// C++
#include <windows.h>
#include<cstdint>
#include<string>
#include<fstream>
#include<sstream>
#include<wrl.h>
using namespace Microsoft::WRL;
#include<memory>
#include<vector>
#include<random>
#include<numbers>

// engine
#include "DirectXGame/engine/Math/MathFanctions.h"


struct ParticleCS {
	Vector3 transrate;
	Vector3 scale;
	Vector3 lifeTime;
	Vector3 velocity;
	float currentTime;
	Vector4 color;
};

struct PreView {
	Matrix4x4 viewProjection;
	Matrix4x4 billboardMatrix;
};

class Entity3DManager;
class EffectManager;
class SrvManager;
class GpuParticleManager {
public:

	// 初期化
	void Initialize(Entity3DManager* entity3DManager, EffectManager* effectManager, SrvManager* srvManager);




private:
	// CS用のパーティクルデータ
	Microsoft::WRL::ComPtr < ID3D12Resource> particleResource_;
	ParticleCS* particleCS_;
	uint32_t particleSrvIndex_ = 0;

	// VS用のビューデータ
	Microsoft::WRL::ComPtr < ID3D12Resource> preViewResource_;
	PreView* preView_;


private:
	Entity3DManager* entity3DManager_;
	EffectManager* effectManager_;
	SrvManager* srvManager_ = nullptr;

};

