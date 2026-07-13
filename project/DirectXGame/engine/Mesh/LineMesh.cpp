#include "LineMesh.h"

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void Engine::LineMesh::Initialize(DirectXCommon* dxcommon)
{
	this->dxCommon = dxcommon;

	// 頂点バッファ生成
	vbvResource_.CreateBufferView(dxCommon, verticesline, verticesline.size());
	// インデクスバッファ生成
	indexResource_.CreateBufferView(dxCommon, indices, indices.size());
}

void Engine::LineMesh::UpdateVertexBuffer()
{
	vbvResource_.UpdateBuffer(verticesline);
}

void Engine::LineMesh::UpdateIndexBuffer()
{
	indexResource_.UpdateBuffer(indices);
}

void Engine::LineMesh::Clear()
{
	indices.clear();
	verticesline.clear();
}

void Engine::LineMesh::GetCommandList()
{
	// 頂点バッファの設定
	vbvResource_.IASetVertexBuffers();
	// インデクスバッファの設定
	indexResource_.IASetIndexBuffer();
}

void Engine::LineMesh::DrawIndexedInstanced()
{
	// バッファ設定
	GetCommandList();

	// 描画コマンド
	if (indices.size() != 0) {
		dxCommon->GetCommandList()->DrawIndexedInstanced(UINT(indices.size()), 1, 0, 0, 0);
	}
}

void Engine::LineMesh::MeshLine(const std::vector<uint32_t>& indices, std::vector<uint32_t>& lineIndices, uint32_t lineNum)
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
