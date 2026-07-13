#include "SkyBoxMesh.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void Engine::SkyBoxMesh::Initialize(DirectXCommon* dxcommon)
{
	// DX共通クラス
	this->dxCommon = dxcommon;
	// 頂点リソース生成
	vbvResource_.CreateBufferView(dxCommon, verticesSkyBox, verticesSkyBox.size());
	// インデックスリソース生成
	indexResource_.CreateBufferView(dxCommon, indices, indices.size());

}

void Engine::SkyBoxMesh::GetCommandList()
{
	// 頂点バッファの設定
	vbvResource_.IASetVertexBuffers();
	// インデックスバッファ設定
	indexResource_.IASetIndexBuffer();

}
