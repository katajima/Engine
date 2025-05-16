#include "LineMesh.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void LineMesh::Initialize(DirectXCommon* dxcommon)
{
	dxCommon_ = dxcommon;

	vertexResource = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(LineVertexData) * verticesline.size());

	// リソースの先頭のアドレスを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(LineVertexData) * verticesline.size());
	vertexBufferView.StrideInBytes = sizeof(LineVertexData);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&lineVertexData));
	std::memcpy(lineVertexData, verticesline.data(), vertexBufferView.SizeInBytes);


	// インデクスリソース
	indexResource = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(uint32_t) * indices.size());

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * indices.size());
	indexBufferView.Format = DXGI_FORMAT_R32_UINT; // インデックスフォーマット

	indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices.data(), indexBufferView.SizeInBytes);
}

void LineMesh::UpdateVertexBuffer()
{
	if (vertexResource) {
		// バッファサイズを確認
		size_t requiredSize = sizeof(LineVertexData) * verticesline.size();
		D3D12_RESOURCE_DESC desc = vertexResource->GetDesc();
		if (requiredSize > desc.Width) {
			// バッファが不足している場合、再割り当て
			vertexResource.Reset();

			D3D12_HEAP_PROPERTIES heapProps = {};
			heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
			D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(requiredSize);
			HRESULT hr = dxCommon_->GetDXGIDevice()->GetDevice()->CreateCommittedResource(
				&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));

			if (FAILED(hr)) {
				// エラー処理
				return;
			}

			// バッファビューの更新
			vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
			vertexBufferView.SizeInBytes = UINT(requiredSize);
			vertexBufferView.StrideInBytes = sizeof(LineVertexData);
		}

		// データのコピー
		LineVertexData* data;
		vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
		memcpy(data, verticesline.data(), requiredSize);
		vertexResource->Unmap(0, nullptr);
	}
}

void LineMesh::Clear()
{
	indices.clear();
	verticesline.clear();
}

void LineMesh::MeshLine(const std::vector<uint32_t>& indices, std::vector<uint32_t>& lineIndices, uint32_t lineNum)
{
	// エッジの重複管理用マップ
	std::unordered_map<uint64_t, bool> edgeMap;

	for (size_t i = 0; i < indices.size(); i += 3) {
		for (int j = 0; j < 3; ++j) {
			uint32_t v0 = indices[i + j] + lineNum;
			uint32_t v1 = indices[i + (j + 1) % 3] + lineNum;

			// 一意なキーを作成（小さい方を前にする）
			if (v0 > v1) std::swap(v0, v1);
			uint64_t edgeKey = (static_cast<uint64_t>(v0) << 32) | v1;

			// まだ登録されていないエッジなら追加
			if (edgeMap.find(edgeKey) == edgeMap.end()) {
				edgeMap[edgeKey] = true;
				lineIndices.push_back(v0);
				lineIndices.push_back(v1);
			}
		}
	}
}
