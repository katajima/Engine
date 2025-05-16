#pragma once
#include "DirectXGame/engine/Mesh/Base/BaseMesh.h"


class LineMesh : public BaseMesh {
public:
	void Initialize(DirectXCommon* dxcommon) override;

	void UpdateVertexBuffer();

	void Clear();

	static void MeshLine(const std::vector<uint32_t>& indices, std::vector<uint32_t>& lineIndices, uint32_t lineNum);
public:
	std::vector<LineVertexData> verticesline;
private:
	// バッファリソース内のデータを指すポインタ
	LineVertexData* lineVertexData = nullptr;
};


