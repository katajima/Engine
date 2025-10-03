#include "TrailMesh.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void TrailMesh::CreateMesh(size_t instanse)
{
	size_t sise = instanse;

	verticesTrail.resize(sise);
	indices.resize(sise);
}

void TrailMesh::Initialize(DirectXCommon* dxcommon)
{
	dxCommon_ = dxcommon;

	
	vbvResorce_.CreateBufferView(dxCommon_, verticesTrail, verticesTrail.size());

	indexResorce_.CreateBufferView(dxCommon_, indices, indices.size());

}

void TrailMesh::GetCommandList()
{
	// 頂点バッファの設定
	vbvResorce_.IASetVertexBuffers();

	indexResorce_.IASetIndexBuffer();

}