#pragma once
#include "DirectXGame/engine/Mesh/Base/BaseMesh.h"

#include "DirectXGame/engine/DirectX/Resource/VertexBufferResource.h"
#include "DirectXGame/engine/DirectX/Resource/IndexBufferResource.h"
#include "DirectXGame/engine/Animation/Animation.h"


namespace Engine {
	// 前方宣言
	class Material;

	/// <summary>
	/// モデルメッシュ
	/// </summary>
	class ModelMesh : public BaseMesh {
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
		/// コマンドリスト設定
		/// </summary>
		void GetCommandListVertex(const D3D12_VERTEX_BUFFER_VIEW& vbv);
		/// <summary>
		/// コマンドリスト設定
		/// </summary>
		void GetCommandList(const D3D12_VERTEX_BUFFER_VIEW& vbv);
		/// <summary>
		/// コマンドリスト設定
		/// </summary>
		void GetCommandList(const D3D12_VERTEX_BUFFER_VIEW& vbv, const D3D12_VERTEX_BUFFER_VIEW& vbv2);
		/// <summary>
		/// 頂点リソース取得
		/// </summary>
		Microsoft::WRL::ComPtr < ID3D12Resource> GetVertexResource() { return vbvResource_.GetVertexResource(); };
		/// <summary>
		/// 頂点バッファビュー取得
		/// </summary>
		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return vbvResource_.GetVertexBufferView(); }


		// メッシュごとのスキンクラスタ
		std::unique_ptr<SkinCluster> skinCluster;
	public:

	public:
		std::vector<VertexData> vertices;
		std::vector<LineVertexData> verticesline;

		std::unique_ptr<Material> material;		// マテリアルデータ
		std::string name;						// メッシュ名
		Vector3 position;						// 位置
	private:
		VertexBuffer<VertexData> vbvResource_;
		IndexBuffer<uint32_t> indexResource_;
	};
}
