#include "TrailMesh.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void Engine::TrailMesh::CreateMesh(size_t instance)
{
	size_t size = instance;

	// リサイズ
	verticesTrail.resize(size);
	indices.resize(size);
}

void Engine::TrailMesh::Initialize(DirectXCommon* dxcommon)
{
	// DX共通クラス
	this->dxCommon = dxcommon;

	// 頂点リソース生成
	vbvResource_.CreateBufferView(dxCommon, verticesTrail, verticesTrail.size());
	// インデックスリソース生成
	indexResource_.CreateBufferView(dxCommon, indices, indices.size());

}

void Engine::TrailMesh::GetCommandList()
{
	// 頂点バッファの設定
	vbvResource_.IASetVertexBuffers();
	// インデックスバッファ設定
	indexResource_.IASetIndexBuffer();

}