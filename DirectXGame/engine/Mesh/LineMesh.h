#pragma once
#include "DirectXGame/engine/Mesh/Base/BaseMesh.h"
#include "DirectXGame/engine/DirectX/Resource/VertexBufferResource.h"
#include "DirectXGame/engine/DirectX/Resource/IndexBufferResource.h"

class LineMesh : public BaseMesh {
public:
	void Initialize(DirectXCommon* dxcommon) override;

	void UpdateVertexBuffer();

	void UpdateIndexBuffer();

	void Clear();

	void GetCommandList() ;



	static void MeshLine(const std::vector<uint32_t>& indices, std::vector<uint32_t>& lineIndices, uint32_t lineNum);
public:
	std::vector<LineVertexData> verticesline;
private:
	VertexBuffer<LineVertexData> vbvResorce_;
	IndexBuffer<uint32_t> indexResorce_;
};


