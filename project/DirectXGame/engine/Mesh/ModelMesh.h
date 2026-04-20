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
		// 初期化
		void Initialize(DirectXCommon* dxcommon) override;
		// 更新頂点バッファ
		void UpdateVertexBuffer();
		// 更新インデクスバッファ
		void UpdateIndexBuffer();
		// クリア
		void Clear();

		// コマンドリスト設定
		void GetCommandList();
		//コマンドリスト設定
		void GetCommandListVertex(const D3D12_VERTEX_BUFFER_VIEW& vbv);
		//コマンドリスト設定
		void GetCommandList(const D3D12_VERTEX_BUFFER_VIEW& vbv);
		//コマンドリスト設定
		void GetCommandList(const D3D12_VERTEX_BUFFER_VIEW& vbv, const D3D12_VERTEX_BUFFER_VIEW& vbv2);
		// 頂点リソース取得
		Microsoft::WRL::ComPtr < ID3D12Resource> GetVertexResource() { return vbvResorce_.GetVertexResource(); };
		// 頂点バッファビュー取得
		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return vbvResorce_.GetVertexBufferView(); }


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
		VertexBuffer<VertexData> vbvResorce_;
		IndexBuffer<uint32_t> indexResorce_;
	};
}
