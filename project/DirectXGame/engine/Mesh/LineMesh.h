#pragma once
#include "DirectXGame/engine/Mesh/Base/BaseMesh.h"
#include "DirectXGame/engine/DirectX/Resource/VertexBufferResource.h"
#include "DirectXGame/engine/DirectX/Resource/IndexBufferResource.h"


/// <summary>
/// ラインメッシュ
/// </summary>
class LineMesh : public BaseMesh {
public:
	// 初期化
	void Initialize(DirectXCommon* dxcommon) override;
	// 更新頂点バッファ
	void UpdateVertexBuffer();
	// 更新インデクスバッファ
	void UpdateIndexBuffer();
	// クリア
	void Clear();
	// コマンドリスト設定
	void GetCommandList() ;
	// 描画
	void DrawIndexedInstanced();


	// ライン
	static void MeshLine(const std::vector<uint32_t>& indices, std::vector<uint32_t>& lineIndices, uint32_t lineNum);
public:
	std::vector<LineVertexData> verticesline;
private:
	VertexBuffer<LineVertexData> vbvResorce_;
	IndexBuffer<uint32_t> indexResorce_;
};


