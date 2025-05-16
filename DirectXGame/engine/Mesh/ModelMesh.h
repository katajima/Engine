#pragma once
#include "DirectXGame/engine/Mesh/Base/BaseMesh.h"

class Material;
class ModelMesh : public BaseMesh {
public:
	void Initialize(DirectXCommon* dxcommon) override;

	void UpdateVertexBuffer();

	void Clear();
public:
	std::vector<VertexData> vertices;
	std::vector<LineVertexData> verticesline;

	std::unique_ptr<Material> material;		// マテリアルデータ
private:
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
};
