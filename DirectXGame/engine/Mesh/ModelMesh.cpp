#include "ModelMesh.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void ModelMesh::Initialize(DirectXCommon* dxcommon)
{
	dxCommon_ = dxcommon;


	vertexResource = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(VertexData) * vertices.size());

	// リソースの先頭のアドレスを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, vertices.data(), sizeof(VertexData) * vertices.size());


	// インデクスリソース
	indexResource = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(uint32_t) * indices.size());

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * indices.size());
	indexBufferView.Format = DXGI_FORMAT_R32_UINT; // インデックスフォーマット

	indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices.data(), sizeof(uint32_t) * indices.size());
}

void ModelMesh::UpdateVertexBuffer()
{
	if (vertexResource) {
		// バッファサイズを確認
		size_t requiredSize = sizeof(VertexData) * vertices.size();
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
			vertexBufferView.StrideInBytes = sizeof(VertexData);
		}

		// データのコピー
		VertexData* data;
		vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
		memcpy(data, vertices.data(), requiredSize);
		vertexResource->Unmap(0, nullptr);
	}
}

void ModelMesh::Clear()
{
	indices.clear();
	vertices.clear();
	verticesline.clear();
}
