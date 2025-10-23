#pragma once

#include "DirectXGame/engine/Mesh/Base/BaseMesh.h"
#include "DirectXGame/engine/DirectX/Resource/VertexBufferResource.h"
#include "DirectXGame/engine/DirectX/Resource/IndexBufferResource.h"

/// <summary>
/// スカイボックスメッシュ
/// </summary>
class SkyBoxMesh : public BaseMesh {
public:
	// 初期化
	void Initialize(DirectXCommon* dxcommon) override;
	// コマンドリスト設定
	void GetCommandList() ;

public:
	std::vector<SkyBoxVertexData> verticesskyBox;
private:
	VertexBuffer<SkyBoxVertexData> vbvResorce_;
	IndexBuffer<uint32_t> indexResorce_;
};
