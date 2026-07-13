#pragma once

#include "DirectXGame/engine/Mesh/Base/BaseMesh.h"
#include "DirectXGame/engine/DirectX/Resource/VertexBufferResource.h"
#include "DirectXGame/engine/DirectX/Resource/IndexBufferResource.h"


namespace Engine {
	/// <summary>
	/// トレイルメッシュ
	/// </summary>
	class TrailMesh : public BaseMesh {
	public:
		// メッシュ生成
		void CreateMesh(size_t instance);
		// 初期化
		void Initialize(DirectXCommon* dxcommon) override;
		// コマンドリスト設定
		void GetCommandList();

	public:
		std::vector<TrailVertexData> verticesTrail;
	private:
		VertexBuffer<TrailVertexData> vbvResource_;
		IndexBuffer<uint32_t> indexResource_;
	};
}