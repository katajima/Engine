#pragma once

#include "DirectXGame/engine/Mesh/Base/BaseMesh.h"
#include "DirectXGame/engine/DirectX/Resource/VertexBufferResource.h"
#include "DirectXGame/engine/DirectX/Resource/IndexBufferResource.h"

class TrailMesh : public BaseMesh {
public:

	void CreateMesh(size_t instanse);

	void Initialize(DirectXCommon* dxcommon) override;

	void GetCommandList();

public:
	std::vector<TrailVertexData> verticesTrail;
private:
	VertexBuffer<TrailVertexData> vbvResorce_;
	IndexBuffer<uint32_t> indexResorce_;
};
