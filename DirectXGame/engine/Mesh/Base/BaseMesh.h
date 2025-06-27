#pragma once

#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/struct/Material.h"
#include "DirectXGame/engine/struct/VertexDeta.h"


#include "vector"
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
using namespace Microsoft::WRL;
#include<d3d12.h>
#include<dxgi1_6.h>
#include "set"
#include <unordered_map>
#include <vector>


class DirectXCommon;
class BaseMesh {
public:
	// 通常用
	virtual void Initialize(DirectXCommon* dxcommon) = 0;

	//void UpdateIndexBuffer();

public:
	// 最小位置
	Vector3 GetMin() const { return min_; }

	// 最大位置
	Vector3 GetMax() const { return max_; }

	// 中心位置
	Vector3 GetMiddle() const { return Vector3{ (max_.x + min_.x) / 2, (max_.y + min_.y) / 2, (max_.z + min_.z) / 2 }; }

	// 最小位置設定
	void SetMin(const Vector3& min) { min_ = min; }

	// 最大位置設定
	void SetMax(const Vector3& max) { max_ = max; }


	
protected:
	DirectXCommon* dxCommon_;


	Vector3 min_;
	Vector3 max_;
public:
	std::vector<Triangle> triangle;
	std::vector<uint32_t> indices;
	std::vector<float> verticesTimer;
	std::vector<float> indicesTimer;
	float maxTime = 0.2f;
	uint32_t meshIndex = 0;
};
