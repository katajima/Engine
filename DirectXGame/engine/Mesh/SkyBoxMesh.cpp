#include "SkyBoxMesh.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void SkyBoxMesh::Initialize(DirectXCommon* dxcommon)
{
	dxCommon_ = dxcommon;

	vbvResorce_.CreateBufferView(dxCommon_, verticesskyBox, verticesskyBox.size());

	indexResorce_.CreateBufferView(dxCommon_, indices, indices.size());

}

void SkyBoxMesh::GetCommandList()
{
	// 頂点バッファの設定
	vbvResorce_.IASetVertexBuffers();

	indexResorce_.IASetIndexBuffer();

}
