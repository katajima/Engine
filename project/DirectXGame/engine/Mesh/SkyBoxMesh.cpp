#include "SkyBoxMesh.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void SkyBoxMesh::Initialize(DirectXCommon* dxcommon)
{
	// DX共通クラス
	dxCommon_ = dxcommon;	
	// 頂点リソース生成
	vbvResorce_.CreateBufferView(dxCommon_, verticesskyBox, verticesskyBox.size());
	// インデックスリソース生成
	indexResorce_.CreateBufferView(dxCommon_, indices, indices.size());

}

void SkyBoxMesh::GetCommandList()
{
	// 頂点バッファの設定
	vbvResorce_.IASetVertexBuffers();
	// インデックスバッファ設定
	indexResorce_.IASetIndexBuffer();

}
