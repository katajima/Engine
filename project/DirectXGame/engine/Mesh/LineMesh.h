#pragma once
#include "DirectXGame/engine/Mesh/Base/BaseMesh.h"
#include "DirectXGame/engine/DirectX/Resource/VertexBufferResource.h"
#include "DirectXGame/engine/DirectX/Resource/IndexBufferResource.h"

namespace Engine {
	/// <summary>
	/// ラインメッシュ
	/// </summary>
	class LineMesh : public BaseMesh {
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* dxcommon) override;
		/// <summary>
		/// 更新頂点バッファ
		/// </summary>
		void UpdateVertexBuffer();
		/// <summary>
		/// 更新インデクスバッファ
		/// </summary>
		void UpdateIndexBuffer();
		/// <summary>
		/// クリア
		/// </summary>
		void Clear();
		/// <summary>
		/// コマンドリスト設定
		/// </summary>
		void GetCommandList();
		/// <summary>
		/// 描画
		/// </summary>
		void DrawIndexedInstanced();


		/// <summary>
		/// ライン
		/// </summary>
		static void MeshLine(const std::vector<uint32_t>& indices, std::vector<uint32_t>& lineIndices, uint32_t lineNum);
	public:
		std::vector<LineVertexData> verticesline;
	private:
		VertexBuffer<LineVertexData> vbvResource_;
		IndexBuffer<uint32_t> indexResource_;
	};
}

