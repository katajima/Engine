#pragma once
#include "GpuParticleData.h"


class LineCommon;
class DirectXCommon;
class GpuParticleField
{
public:
	void Init(DirectXCommon* dxCommon,LineCommon* lineCommon,std::string name);

	void UpdateImgui();

	void Update();

private:
	// パーティクル影響場所
	ConstantBuffer<EffectFieldCS> cbEffectFieldResource_;


	std::string name_;
	LineCommon* lineCommon_;
	DirectXCommon* dxCommon_;
};

