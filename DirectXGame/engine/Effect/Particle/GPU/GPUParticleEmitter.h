#pragma once

#include "GpuParticleData.h"

class DirectXCommon;
class LineCommon;
class GpuParticleGroup;
class GpuParticleEmitter
{
public:
	void Init(DirectXCommon* dxCommon, LineCommon* lineCommon, GpuParticleGroup* group,std::string name);

	void Update(float deltaTime);

	void UpdateImGui();


	void SetParticleGroup(GpuParticleGroup* group);

	EmitterSphere* GetData() { return cbEmitterSphere_.Data();}

private:
	// 球エミッター
	ConstantBuffer<EmitterSphere> cbEmitterSphere_;
	std::string name_;

	bool isEmitte_ = true; 


	int count_ = 64;

	DirectXCommon* dxCommon_ = nullptr;		// DirectX共通クラス
	GpuParticleGroup* group_ = nullptr;		// GPUパーティクルグループ
	LineCommon* lineCommon_;
};

