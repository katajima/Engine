#include "LineCommon.h"
#include "combaseapi.h"
#include "DirectXGame/engine/base/SrvManager.h"
#include "DirectXGame/engine/Material/Material.h"


LineCommon* LineCommon::instance = nullptr;


LineCommon* LineCommon::GetInstance()
{
	if (instance == nullptr) {
		instance = new LineCommon;
	}
	return instance;
}

void LineCommon::Finalize()
{
	delete instance;
	instance = nullptr;
}

void LineCommon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	CreateGraphicsPipeline();

	
	// マテリアル
	materialResource = dxCommon_->CreateBufferResource(sizeof(Material));
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	//今回は赤を書き込んで見る //白
	*materialData = MaterialData({ 1.0f, 1.0f, 1.0f, 1.0f }); //RGBA



	viewResource = dxCommon_->CreateBufferResource(sizeof(Matrix4x4));
	viewResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraWVP));

	*cameraWVP = MakeIdentity4x4();

	
	mesh_ = std::make_unique<Mesh>();
	mesh_->verticesline.push_back({ 0,0,0,0 });
	mesh_->verticesline.push_back({ 0,0,0,0 });
	mesh_->indices.push_back({ 0 });
	mesh_->indices.push_back({ 1 });
	mesh_->InitializeLine(LineCommon::GetInstance()->GetDxCommon());

	mesh_->verticesline.clear();
	mesh_->indices.clear();
}

void LineCommon::AddLine(Vector3 start, Vector3 end, Vector4 color)
{
	mesh_->verticesline.push_back({ { start.x, start.y, start.z, 1.0f }, color });
	mesh_->verticesline.push_back({ { end.x, end.y, end.z, 1.0f }, color });

	mesh_->indices.push_back(lineNum_);
	mesh_->indices.push_back(lineNum_ + 1);

	lineNum_ += 2;
}

void LineCommon::AddPointLightLine(PointLightData data)
{
	constexpr int segments = 16; // 1つの円を構成するセグメント数
	const float radius = data.radius; // ポイントライトの届く距離

	Vector3 center = data.position;
	Vector4 lineColor = data.color; // ライトの色をそのまま使用

	// 3つの軸 (XY, XZ, YZ) に円を描画
	for (int axis = 0; axis < 3; ++axis) {
		for (int i = 0; i < segments; ++i) {
			float theta1 = (2.0f * static_cast<float>(M_PI) * i) / segments;
			float theta2 = (2.0f * static_cast<float>(M_PI) * (i + 1)) / segments;

			Vector3 p1, p2;

			if (axis == 0) { // XY平面
				p1 = Vector3(radius * cosf(theta1), radius * sinf(theta1), 0.0f);
				p2 = Vector3(radius * cosf(theta2), radius * sinf(theta2), 0.0f);
			}
			else if (axis == 1) { // XZ平面
				p1 = Vector3(radius * cosf(theta1), 0.0f, radius * sinf(theta1));
				p2 = Vector3(radius * cosf(theta2), 0.0f, radius * sinf(theta2));
			}
			else { // YZ平面
				p1 = Vector3(0.0f, radius * cosf(theta1), radius * sinf(theta1));
				p2 = Vector3(0.0f, radius * cosf(theta2), radius * sinf(theta2));
			}

			// 中心座標をオフセット
			p1 += center;
			p2 += center;

			// 頂点を追加
			mesh_->verticesline.push_back({ { p1.x, p1.y, p1.z, 1.0f }, lineColor });
			mesh_->verticesline.push_back({ { p2.x, p2.y, p2.z, 1.0f }, lineColor });

			// インデックスを追加
			mesh_->indices.push_back(lineNum_);
			mesh_->indices.push_back(lineNum_ + 1);

			lineNum_ += 2;
		}
	}

	// XYZ 軸方向に intensity の強さを反映したラインを追加
	Vector3 intensityVectorX = { data.intensity, 0.0f, 0.0f };
	Vector3 intensityVectorY = { 0.0f, data.intensity, 0.0f };
	Vector3 intensityVectorZ = { 0.0f, 0.0f, data.intensity };

	Vector3 xStart = center - intensityVectorX;
	Vector3 xEnd = center + intensityVectorX;
	Vector3 yStart = center - intensityVectorY;
	Vector3 yEnd = center + intensityVectorY;
	Vector3 zStart = center - intensityVectorZ;
	Vector3 zEnd = center + intensityVectorZ;

	// 各軸のラインを描画
	AddLine(xStart, xEnd, lineColor); // X軸
	AddLine(yStart, yEnd, lineColor); // Y軸
	AddLine(zStart, zEnd, lineColor); // Z軸
}


void LineCommon::AddLineMesh(Mesh* mesh, const Matrix4x4& worldMat)
{



	for (size_t i = 0; i < mesh->verticesline.size(); i++) {
		// ワールド行列を適用して変換
		Vector4 pos = mesh->verticesline[i].position;
		Vector4 worldPos = Transforms(pos, worldMat);

		mesh_->verticesline.push_back({ worldPos.x, worldPos.y, worldPos.z, 1 });
	}

	Mesh::MeshLine(mesh->indices,mesh_->indices, lineNum_);


	// インデックスオフセットを更新
	lineNum_ += static_cast<uint32_t>(mesh_->indices.size());
}

void LineCommon::AddLineMesh(Mesh* mesh, const Matrix4x4& worldMat, std::vector<uint32_t> cachedLineIndices)
{


	for (size_t i = 0; i < mesh->verticesline.size(); i++) {
		// ワールド行列を適用して変換
		Vector4 pos = mesh->verticesline[i].position;
		Vector4 worldPos = Transforms(pos, worldMat);

		mesh_->verticesline.push_back({ worldPos.x, worldPos.y, worldPos.z, 1 });
	}

	for (size_t i = 0; i < cachedLineIndices.size(); i++) {
		mesh_->indices.push_back(cachedLineIndices[i + lineNum_]);
	}
	

	// インデックスオフセットを更新
	lineNum_ += static_cast<uint32_t>(mesh_->indices.size());
}

void LineCommon::Update()
{
	mesh_->UpdateLineVertexBuffer();
	mesh_->UpdateIndexBuffer();

	if (camera_ && cameraWVP) {
		*cameraWVP = camera_->viewProjectionMatrix_;  // データをコピー
	}
}

void LineCommon::DrawCommonSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
}

void LineCommon::Draw()
{
	DrawCommonSetting();

	auto commandList = dxCommon_->GetCommandList();

	// SRV (インスタンシングデータ) をルートパラメータ [0] に設定
	commandList->SetGraphicsRootConstantBufferView(1, materialResource->GetGPUVirtualAddress());

	// ビューデータ
	commandList->SetGraphicsRootConstantBufferView(0, viewResource->GetGPUVirtualAddress());


	mesh_->GetCommandList();

	
	//commandList->DrawInstanced(UINT(mesh_->verticesline.size()), 1, 0, 0);
	commandList->DrawIndexedInstanced(UINT(mesh_->indices.size()), 1, 0, 0, 0);

}

void LineCommon::LineClear()
{
	lineNum_ = 0;
	mesh_->Clear();
}

void LineCommon::CreateRootSignature()
{
	HRESULT hr;

	D3D12_ROOT_PARAMETER rootParameters[2] = {};  // 2つのパラメーターを使う
	// [1] は SRV (インスタンシングデータ用)

	CameraCommon::SetRootParameterVertex(rootParameters[0], 0);

	// マテリアルデータ (b0) をピクセルシェーダで使用する
	Material::SetRootParameter(rootParameters[1], 1);

	D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
	descriptionSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	descriptionSignature.pParameters = rootParameters;
	descriptionSignature.NumParameters = _countof(rootParameters);

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));
}


void LineCommon::CreateGraphicsPipeline()
{
	HRESULT hr;
	CreateRootSignature();


	// InputLayout(インプットレイアウト)
	// VectorShaderへ渡す頂点データがどのようなものかを指定するオブジェクト


#pragma region InputLayout


	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "COLOR";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


#pragma endregion //InputLayout(インプットレイアウト)


	// BlendState(ブレンドステート)の設定
	// PixeiShaderからの出力をどのように書き込むかを設定する
	//半透明処理はここの設定により制御され、書き込む色要素を決めることができる


#pragma region BlendState


	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

#pragma endregion //BlendState(ブレンドステート)


	// RasterizerState(ラスタライザステート)の設定
	// 三角形の内部をピクセルに分解して、PixelShaderを起動することで、この処理の設定

	D3D12_RASTERIZER_DESC rasterizerDesc{};

	//裏面(時計回り)を表示しない
	// カリングしない(裏面も表示させる)
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;

	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = dxCommon_->CompileShader(L"resources/shaders/Line/Line.VS.hlsl",
		L"vs_6_0");

	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = dxCommon_->CompileShader(L"resources/shaders/Line/Line.PS.hlsl",
		L"ps_6_0");

	assert(pixelShaderBlob != nullptr);


	// PSOを作成する

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();// RootSignature

	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;// InputLayout

	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() }; // VertexShader

	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() }; // PixelShader

	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState

	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;// RasterizerState

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトロポジ(形状)のタイプ。線形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;



	//DepthStencilStateの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;


	hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));

	assert(SUCCEEDED(hr));


}
