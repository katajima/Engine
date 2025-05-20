#pragma once

#include "DirectXGame/engine/Mesh/Base/BaseMesh.h"
#include "DirectXGame/engine/DirectX/Resource/VertexBufferResource.h"
#include "DirectXGame/engine/DirectX/Resource/IndexBufferResource.h"

class SkyBoxMesh : public BaseMesh {
public:
	void Initialize(DirectXCommon* dxcommon) override;

	void GetCommandList() ;

public:
	std::vector<SkyBoxVertexData> verticesskyBox;
private:
	VertexBuffer<SkyBoxVertexData> vbvResorce_;
	IndexBuffer<uint32_t> indexResorce_;
};
