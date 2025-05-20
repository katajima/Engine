#include "ModelMesh.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void ModelMesh::Initialize(DirectXCommon* dxcommon)
{
	dxCommon_ = dxcommon;

	vbvResorce_.CreateBufferView(dxCommon_, vertices, vertices.size());

	indexResorce_.CreateBufferView(dxCommon_, indices, indices.size());

}

void ModelMesh::UpdateVertexBuffer()
{
	vbvResorce_.UpdateBuffer(vertices);
}

void ModelMesh::UpdateIndexBuffer()
{
	indexResorce_.UpdateBuffer(indices);
}

void ModelMesh::Clear()
{
	indices.clear();
	vertices.clear();
	verticesline.clear();
}

void ModelMesh::GetCommandList()
{
	// 頂点バッファの設定
	vbvResorce_.IASetVertexBuffers();

	indexResorce_.IASetIndexBuffer();

}

void ModelMesh::GetCommandListVertex(const D3D12_VERTEX_BUFFER_VIEW& vbv)
{
	vbvResorce_.IASetVertexBuffers(vbv);
	
	indexResorce_.IASetIndexBuffer();

}


void ModelMesh::GetCommandList(const D3D12_VERTEX_BUFFER_VIEW& vbv)
{

	vbvResorce_.IASetVertexBuffersSlot(vbv);

	indexResorce_.IASetIndexBuffer();

}

void ModelMesh::GetCommandList(const D3D12_VERTEX_BUFFER_VIEW& vbv, const D3D12_VERTEX_BUFFER_VIEW& vbv2)
{
	vbvResorce_.IASetVertexBuffersSlot(vbv,vbv2);
	indexResorce_.IASetIndexBuffer();
	
}
