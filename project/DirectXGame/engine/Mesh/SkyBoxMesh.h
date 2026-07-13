#pragma once

#include "DirectXGame/engine/Mesh/Base/BaseMesh.h"
#include "DirectXGame/engine/DirectX/Resource/VertexBufferResource.h"
#include "DirectXGame/engine/DirectX/Resource/IndexBufferResource.h"


namespace Engine {
	/// <summary>
	/// スカイボックスメッシュ
	/// </summary>
	class SkyBoxMesh : public BaseMesh {
	public:
		// 初期化
		void Initialize(DirectXCommon* dxcommon) override;
		// コマンドリスト設定
		void GetCommandList();

	public:
		std::vector<SkyBoxVertexData> verticesSkyBox;
	private:
		VertexBuffer<SkyBoxVertexData> vbvResource_;
		IndexBuffer<uint32_t> indexResource_;
	};
}