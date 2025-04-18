#pragma once
#include"DirectXGame/engine/base/TextureManager.h"

#include "DirectXGame/engine/struct/Structs3D.h"

class DirectXCommon;
class RenderingCommon
{
public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	void DrawCommonSetting(int index);



private:
	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();
private:
	//頂点データ
	struct VertexData {

		Vector4 position;
		Vector2 texcoord;
	};

	DirectXCommon* dxCommon_;

	//ルートシグネチャデスク
	D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
	//ルートシグネチャ
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
	// グラフィックスパイプラインステート
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState = nullptr;

	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	Microsoft::WRL::ComPtr < ID3D12Resource> vertexResource;
	VertexData* vertexData = nullptr;

};


