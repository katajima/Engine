#include "LineMesh.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void LineMesh::Initialize(DirectXCommon* dxcommon)
{
	dxCommon_ = dxcommon;

	// 頂点バッファ生成
	vbvResorce_.CreateBufferView(dxCommon_, verticesline, verticesline.size());
	// インデクスバッファ生成
	indexResorce_.CreateBufferView(dxCommon_, indices, indices.size());
}

void LineMesh::UpdateVertexBuffer()
{
	vbvResorce_.UpdateBuffer(verticesline);
}

void LineMesh::UpdateIndexBuffer()
{
	indexResorce_.UpdateBuffer(indices);
}

void LineMesh::Clear()
{
	indices.clear();
	verticesline.clear();
}

void LineMesh::GetCommandList()
{
	// 頂点バッファの設定
	vbvResorce_.IASetVertexBuffers();
	// インデクスバッファの設定
	indexResorce_.IASetIndexBuffer();
}

void LineMesh::DrawIndexedInstanced()
{
	// バッファ設定
	GetCommandList();

	// 描画コマンド
	if (indices.size() != 0) {
		dxCommon_->GetCommandList()->DrawIndexedInstanced(UINT(indices.size()), 1, 0, 0, 0);
	}
}

void LineMesh::MeshLine(const std::vector<uint32_t>& indices, std::vector<uint32_t>& lineIndices, uint32_t lineNum)
{
	// エッジの重複管理用マップ
	std::unordered_map<uint64_t, bool> edgeMap;

	for (size_t i = 0; i < indices.size(); i += 3) {
		for (int j = 0; j < 3; ++j) {
			uint32_t v0 = indices[i + j] + lineNum;
			uint32_t v1 = indices[i + (j + 1) % 3] + lineNum;

			// 一意なキーを作成（小さい方を前にする）
			if (v0 > v1) std::swap(v0, v1);
			uint64_t edgeKey = (static_cast<uint64_t>(v0) << 32) | v1;

			// まだ登録されていないエッジなら追加
			if (edgeMap.find(edgeKey) == edgeMap.end()) {
				edgeMap[edgeKey] = true;
				lineIndices.push_back(v0);
				lineIndices.push_back(v1);
			}
		}
	}
}
