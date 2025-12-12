#include "ModelMesh.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void Engine::ModelMesh::Initialize(DirectXCommon* dxcommon)
{
	// DX共通クラス
	dxCommon_ = dxcommon;
	// 頂点リソース生成
	vbvResorce_.CreateBufferView(dxCommon_, vertices, vertices.size());
	// インデックスリソース生成
	indexResorce_.CreateBufferView(dxCommon_, indices, indices.size());
}

void Engine::ModelMesh::UpdateVertexBuffer()
{
	// 頂点リソース更新
	vbvResorce_.UpdateBuffer(vertices);
}

void Engine::ModelMesh::UpdateIndexBuffer()
{
	// インデックスリソース更新
	indexResorce_.UpdateBuffer(indices);
}

void Engine::ModelMesh::Clear()
{
	indices.clear();
	vertices.clear();
	verticesline.clear();
}

void Engine::ModelMesh::GetCommandList()
{
	// 頂点バッファの設定
	vbvResorce_.IASetVertexBuffers();
	// インデックスバッファ設定
	indexResorce_.IASetIndexBuffer();

}

void Engine::ModelMesh::GetCommandListVertex(const D3D12_VERTEX_BUFFER_VIEW& vbv)
{
	// 頂点バッファの設定
	vbvResorce_.IASetVertexBuffers(vbv);
	// インデックスバッファ設定
	indexResorce_.IASetIndexBuffer();
}


void Engine::ModelMesh::GetCommandList(const D3D12_VERTEX_BUFFER_VIEW& vbv)
{
	// 頂点バッファの設定
	vbvResorce_.IASetVertexBuffersSlot(vbv);
	// インデックスバッファ設定
	indexResorce_.IASetIndexBuffer();
}

void Engine::ModelMesh::GetCommandList(const D3D12_VERTEX_BUFFER_VIEW& vbv, const D3D12_VERTEX_BUFFER_VIEW& vbv2)
{
	// 頂点バッファの設定
	vbvResorce_.IASetVertexBuffersSlot(vbv,vbv2);
	// インデックスバッファ設定
	indexResorce_.IASetIndexBuffer();
}
