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
		/// <summary>
		/// メッシュ生成
		/// </summary>
		void CreateMesh(size_t instance);
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* dxcommon) override;
		/// <summary>
		/// コマンドリスト設定
		/// </summary>
		void GetCommandList();

	public:
		std::vector<TrailVertexData> verticesTrail;
	private:
		VertexBuffer<TrailVertexData> vbvResource_;
		IndexBuffer<uint32_t> indexResource_;
	};
}