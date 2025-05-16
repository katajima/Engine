#include "BaseMesh.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void BaseMesh::UpdateIndexBuffer()
{
	if (indexResource) {
		// バッファサイズを確認
		size_t requiredSize = sizeof(uint32_t) * indices.size();
		D3D12_RESOURCE_DESC desc = indexResource->GetDesc();
		if (requiredSize > desc.Width) {
			// バッファが不足している場合、再割り当て
			indexResource.Reset();

			D3D12_HEAP_PROPERTIES heapProps = {};
			heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
			D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(requiredSize);
			HRESULT hr = dxCommon_->GetDXGIDevice()->GetDevice()->CreateCommittedResource(
				&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indexResource));

			if (FAILED(hr)) {
				// エラー処理
				return;
			}

			// バッファビューの更新
			indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
			indexBufferView.SizeInBytes = UINT(requiredSize);

		}
		// データのコピー
		uint32_t* data;
		indexResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
		memcpy(data, indices.data(), requiredSize);
		indexResource->Unmap(0, nullptr);
	}
}

void BaseMesh::GetCommandList()
{
	// 頂点バッファの設定
	dxCommon_->GetCommand()->GetList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	// インデックスバッファの設定
	dxCommon_->GetCommand()->GetList()->IASetIndexBuffer(&indexBufferView);
}

void BaseMesh::GetCommandListVertex(const D3D12_VERTEX_BUFFER_VIEW& vbv)
{
	// 頂点バッファの設定
	dxCommon_->GetCommand()->GetList()->IASetVertexBuffers(0, 1, &vbv);
	// インデックスバッファの設定
	dxCommon_->GetCommand()->GetList()->IASetIndexBuffer(&indexBufferView);
}

void BaseMesh::GetCommandList(const D3D12_VERTEX_BUFFER_VIEW& vbv)
{
	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
	vertexBufferView,
	vbv
	};

	// 頂点バッファの設定
	dxCommon_->GetCommand()->GetList()->IASetVertexBuffers(0, 2, vbvs);
	// インデックスバッファの設定
	dxCommon_->GetCommand()->GetList()->IASetIndexBuffer(&indexBufferView);
}

void BaseMesh::GetCommandList(const D3D12_VERTEX_BUFFER_VIEW& vbv, const D3D12_VERTEX_BUFFER_VIEW& vbv2)
{
	D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
	vbv,
	vbv2
	};

	// 頂点バッファの設定
	dxCommon_->GetCommand()->GetList()->IASetVertexBuffers(0, 2, vbvs);
	// インデックスバッファの設定
	dxCommon_->GetCommand()->GetList()->IASetIndexBuffer(&indexBufferView);
}
