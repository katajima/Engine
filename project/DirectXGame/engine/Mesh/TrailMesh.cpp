#include "TrailMesh.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void Engine::TrailMesh::CreateMesh(size_t instanse)
{
	size_t sise = instanse;

	// リサイズ
	verticesTrail.resize(sise);
	indices.resize(sise);
}

void Engine::TrailMesh::Initialize(DirectXCommon* dxcommon)
{
	// DX共通クラス
	dxCommon_ = dxcommon;

	// 頂点リソース生成
	vbvResorce_.CreateBufferView(dxCommon_, verticesTrail, verticesTrail.size());
	// インデックスリソース生成
	indexResorce_.CreateBufferView(dxCommon_, indices, indices.size());

}

void Engine::TrailMesh::GetCommandList()
{
	// 頂点バッファの設定
	vbvResorce_.IASetVertexBuffers();
	// インデックスバッファ設定
	indexResorce_.IASetIndexBuffer();

}