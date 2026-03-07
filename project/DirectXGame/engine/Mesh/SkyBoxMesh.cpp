#include "SkyBoxMesh.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void Engine::SkyBoxMesh::Initialize(DirectXCommon* dxcommon)
{
	// DX共通クラス
	this->dxCommon = dxcommon;	
	// 頂点リソース生成
	vbvResorce_.CreateBufferView(dxCommon, verticesskyBox, verticesskyBox.size());
	// インデックスリソース生成
	indexResorce_.CreateBufferView(dxCommon, indices, indices.size());

}

void Engine::SkyBoxMesh::GetCommandList()
{
	// 頂点バッファの設定
	vbvResorce_.IASetVertexBuffers();
	// インデックスバッファ設定
	indexResorce_.IASetIndexBuffer();

}
