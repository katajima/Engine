#pragma once
#include "DirectXGame/engine/Mesh/Base/BaseMesh.h"

#include "DirectXGame/engine/DirectX/Resource/VertexBufferResource.h"
#include "DirectXGame/engine/DirectX/Resource/IndexBufferResource.h"
#include "DirectXGame/engine/Animation/Animation.h"

class Material;
class ModelMesh : public BaseMesh {
public:
	void Initialize(DirectXCommon* dxcommon) override;

	void UpdateVertexBuffer();

	void UpdateIndexBuffer();

	void Clear();


	void GetCommandList() ;

	void GetCommandListVertex(const D3D12_VERTEX_BUFFER_VIEW& vbv);

	void GetCommandList(const D3D12_VERTEX_BUFFER_VIEW& vbv);
	void GetCommandList(const D3D12_VERTEX_BUFFER_VIEW& vbv, const D3D12_VERTEX_BUFFER_VIEW& vbv2);

	Microsoft::WRL::ComPtr < ID3D12Resource> GetVertexResource() { return vbvResorce_.GetVertexResource(); };

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return vbvResorce_.GetVertexBufferView(); }


	// メッシュごとのスキンクラスタ
	std::unique_ptr<SkinCluster> skinCluster;

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


struct MeshData
{

};