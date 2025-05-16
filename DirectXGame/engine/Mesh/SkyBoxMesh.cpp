#include "SkyBoxMesh.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void SkyBoxMesh::Initialize(DirectXCommon* dxcommon)
{
	dxCommon_ = dxcommon;

	vertexResource = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(SkyBoxVertexData) * verticesskyBox.size());

	// リソースの先頭のアドレスを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(SkyBoxVertexData) * verticesskyBox.size());
	vertexBufferView.StrideInBytes = sizeof(SkyBoxVertexData);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&skyBoxVertexData));
	std::memcpy(skyBoxVertexData, verticesskyBox.data(), vertexBufferView.SizeInBytes);


	// インデクスリソース
	indexResource = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(uint32_t) * indices.size());

	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = UINT(sizeof(uint32_t) * indices.size());
	indexBufferView.Format = DXGI_FORMAT_R32_UINT; // インデックスフォーマット

	indexData = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));
	std::memcpy(indexData, indices.data(), indexBufferView.SizeInBytes);
}
