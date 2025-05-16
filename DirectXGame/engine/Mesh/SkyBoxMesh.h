#pragma once

#include "DirectXGame/engine/Mesh/Base/BaseMesh.h"


class SkyBoxMesh : public BaseMesh {
public:
	void Initialize(DirectXCommon* dxcommon) override;

public:
	std::vector<SkyBoxVertexData> verticesskyBox;
private:
	// バッファリソース内のデータを指すポインタ
	SkyBoxVertexData* skyBoxVertexData = nullptr;
};
