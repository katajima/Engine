#pragma once

#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/struct/Material.h"
#include "DirectXGame/engine/collider/3d/ColliderFanction3D.h"
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

	void UpdateIndexBuffer();

	// コマンドリスト
	void GetCommandList();

	void GetCommandListVertex(const D3D12_VERTEX_BUFFER_VIEW& vbv);

	void GetCommandList(const D3D12_VERTEX_BUFFER_VIEW& vbv);
	void GetCommandList(const D3D12_VERTEX_BUFFER_VIEW& vbv, const D3D12_VERTEX_BUFFER_VIEW& vbv2);
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


	Microsoft::WRL::ComPtr < ID3D12Resource> GetVertexResource() { return vertexResource; };

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return vertexBufferView; }
protected:
	DirectXCommon* dxCommon_;

	// バッファリソース
	Microsoft::WRL::ComPtr < ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr < ID3D12Resource> indexResource;

	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	uint32_t* indexData;

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
