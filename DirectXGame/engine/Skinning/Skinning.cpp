#include "Skinning.h"

#include "DirectXGame/engine/Transfomation/Transfomation.h"
#include "DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/Light/LightCommon.h"
#include "DirectXGame/engine/Camera/CameraCommon.h"
#include "DirectXGame/engine/base/TextureManager.h"

SkinningConmmon* SkinningConmmon::instance = nullptr;

SkinningConmmon* SkinningConmmon::GetInstance()
{
	if (instance == nullptr) {
		instance = new SkinningConmmon;
	}
	return instance;
}

void SkinningConmmon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_);

	CreateGraphicsPipeline();
}

void SkinningConmmon::Finalize()
{
	delete instance;
	instance = nullptr;
}

void SkinningConmmon::DrawCommonSetting(PSOType type)
{
	switch (type)
	{
	case SkinningConmmon::PSOType::UvInterpolation_MODE_SOLID_BACK:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[0].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[0].Get());
		break;
	case SkinningConmmon::PSOType::NoUvInterpolation_MODE_SOLID_BACK:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[1].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[1].Get());
		break;
	case SkinningConmmon::PSOType::UvInterpolation_MODE_WIREFRAME_BACK:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[0].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[2].Get());
		break;
	case SkinningConmmon::PSOType::NoUvInterpolation_MODE_WIREFRAME_BACK:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[1].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[3].Get());
		break;
	case SkinningConmmon::PSOType::UvInterpolation_MODE_SOLID_NONE:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[0].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[4].Get());
		break;
	case SkinningConmmon::PSOType::NoUvInterpolation_MODE_SOLID_NONE:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[1].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[5].Get());
		break;
	case SkinningConmmon::PSOType::UvInterpolation_MODE_WIREFRAME_NONE:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[0].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[6].Get());
		break;
	case SkinningConmmon::PSOType::NoUvInterpolation_MODE_WIREFRAME_NONE:
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature[1].Get());
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState[7].Get());
		break;
	default:
		break;
	}
}

void SkinningConmmon::CreateRootSignature()
{
	
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0; // 0から始まる
	descriptorRange[0].NumDescriptors = 1; // 数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算

	D3D12_DESCRIPTOR_RANGE descriptorRangeNormalmap[1] = {};
	descriptorRangeNormalmap[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeNormalmap[0].NumDescriptors = 1;
	descriptorRangeNormalmap[0].BaseShaderRegister = 1;
	descriptorRangeNormalmap[0].RegisterSpace = 0;
	descriptorRangeNormalmap[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE descriptorRangeSpecualrmap[1] = {}; // スペキュラマップ (t2) をピクセルシェーダで使用するための設定 
	descriptorRangeSpecualrmap[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV (Shader Resource View) を使用する 
	descriptorRangeSpecualrmap[0].NumDescriptors = 1; // 使用するデスクリプタの数 
	descriptorRangeSpecualrmap[0].BaseShaderRegister = 2; // t1 レジスタにバインド 
	descriptorRangeSpecualrmap[0].RegisterSpace = 0; // レジスタスペース (通常は0) 
	descriptorRangeSpecualrmap[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE descriptorRangeAoMap[1] = {}; // スペキュラマップ (t2) をピクセルシェーダで使用するための設定 
	descriptorRangeAoMap[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRV (Shader Resource View) を使用する 
	descriptorRangeAoMap[0].NumDescriptors = 1; // 使用するデスクリプタの数 
	descriptorRangeAoMap[0].BaseShaderRegister = 3; // t1 レジスタにバインド 
	descriptorRangeAoMap[0].RegisterSpace = 0; // レジスタスペース (通常は0) 
	descriptorRangeAoMap[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// スキニング用
	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 4; // 0から始まる
	descriptorRangeForInstancing[0].NumDescriptors = 1; // 数は1つ
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // SRVを使う
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算


	// Roosignature(ルートシグネチャ)作成
	//ShaderとResorceをどのように関連付けるかを示したオブジェクト

	//Roosignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};

	descriptionSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// RootParameter作成。複数指定できるのではい
	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[11] = {};

	// マテリアルデータ (b0) をピクセルシェーダで使用する
	Material::SetRootParameter(rootParameters[0], 0);

	// トランスフォームデータ (b0) を頂点シェーダで使用する
	Transfomation::SetRootParameter(rootParameters[1], 0);

	// テクスチャデータ (t0) をピクセルシェーダで使用する
	TextureManager::SetRootParameter(rootParameters[2], descriptorRange[0]);

	// 方向性ライトデータ (b1) をピクセルシェーダで使用する
	LightManager::SetRootParameter(rootParameters[3], 1);

	// カメラデータ (b2) をピクセルシェーダで使用する
	CameraCommon::SetRootParameter(rootParameters[4], 2);

	// ポイントライトデータ (b3) をピクセルシェーダで使用する
	LightManager::SetRootParameter(rootParameters[5], 3);

	// スポットライトデータ (b4) をピクセルシェーダで使用する
	LightManager::SetRootParameter(rootParameters[6], 4);

	// 法線マップデータ (t1) をピクセルシェーダで使用する 
	TextureManager::SetRootParameter(rootParameters[7], descriptorRangeNormalmap[0]);

	// 法線マップデータ (t2) をピクセルシェーダで使用する 
	TextureManager::SetRootParameter(rootParameters[8], descriptorRangeSpecualrmap[0]);

	// 法線マップデータ (t2) をピクセルシェーダで使用する 
	TextureManager::SetRootParameter(rootParameters[9], descriptorRangeAoMap[0]);
	// スキニング (t4)
	rootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[10].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
	rootParameters[10].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);


	descriptionSignature.pParameters = rootParameters;
	descriptionSignature.NumParameters = _countof(rootParameters);


	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0～1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0; //レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う


	descriptionSignature.pStaticSamplers = staticSamplers;
	descriptionSignature.NumStaticSamplers = _countof(staticSamplers);

	Blob(dxCommon_, descriptionSignature, rootSignature[0]);


	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT; // バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0～1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0; //レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
	descriptionSignature.pStaticSamplers = staticSamplers;
	descriptionSignature.NumStaticSamplers = _countof(staticSamplers);


	Blob(dxCommon_, descriptionSignature, rootSignature[1]);




	//D3D12_DESCRIPTOR_RANGE computeDescriptorRange[3] = {};
	//psoManager_->SetDescriptorRenge(computeDescriptorRange[0], 0, 1,D3D12_DESCRIPTOR_RANGE_TYPE_SRV); //Palette
	//psoManager_->SetDescriptorRenge(computeDescriptorRange[1], 1, 1,D3D12_DESCRIPTOR_RANGE_TYPE_SRV); //InputVertices
	//psoManager_->SetDescriptorRenge(computeDescriptorRange[2], 2, 1,D3D12_DESCRIPTOR_RANGE_TYPE_SRV); //Influence



	//D3D12_ROOT_PARAMETER computeRootParameters[5] = {};

	//psoManager_->SetRootParameter(computeRootParameters[0],0,D3D12_SHADER_VISIBILITY_ALL,D3D12_ROOT_PARAMETER_TYPE_CBV); // gSkinningInfomation

	//psoManager_->SetRootParameter(computeRootParameters[1],computeDescriptorRange[0], D3D12_SHADER_VISIBILITY_ALL);
	//psoManager_->SetRootParameter(computeRootParameters[2],computeDescriptorRange[1], D3D12_SHADER_VISIBILITY_ALL);
	//psoManager_->SetRootParameter(computeRootParameters[3],computeDescriptorRange[2], D3D12_SHADER_VISIBILITY_ALL);
	//psoManager_->SetRootParameter(computeRootParameters[4], 0, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_UAV); // gSkinningInfomation


	//
	////HRESULT hr;
	//// Shaderをコンパイルする
	//Microsoft::WRL::ComPtr < IDxcBlob> CSBlob = dxCommon_->CompileShader(L"resources/shaders/Skining/Skinning.CS.hlsl", L"cs_6_0");
	//D3D12_COMPUTE_PIPELINE_STATE_DESC computePipelineStatedesc{};
	//computePipelineStatedesc.CS = {
	//	.pShaderBytecode = CSBlob->GetBufferPointer(),
	//	.BytecodeLength = CSBlob->GetBufferSize()
	//};
	/*computePipelineStatedesc.pRootSignature = computeRootSignature.Get();
	Microsoft::WRL::ComPtr<ID3D12PipelineState> computePipelineState = nullptr;
	hr = dxCommon_->GetDevice()->CreateComputePipelineState(&computePipelineStatedesc,IID_PPV_ARGS(&computePipelineState));*/

}

void SkinningConmmon::CreateGraphicsPipeline()
{
	CreateRootSignature();



#pragma region BlendState

	// BlendState(ブレンドステート)の設定
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
	D3D12_RASTERIZER_DESC rasterizerDesc{};

	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;

	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	GraphicsPipelineState(rootSignature[0], graphicsPipelineState[0], rasterizerDesc, blendDesc);
	GraphicsPipelineState(rootSignature[1], graphicsPipelineState[1], rasterizerDesc, blendDesc);
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	GraphicsPipelineState(rootSignature[0], graphicsPipelineState[2], rasterizerDesc, blendDesc);
	GraphicsPipelineState(rootSignature[1], graphicsPipelineState[3], rasterizerDesc, blendDesc);


	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;

	GraphicsPipelineState(rootSignature[0], graphicsPipelineState[4], rasterizerDesc, blendDesc);
	GraphicsPipelineState(rootSignature[1], graphicsPipelineState[5], rasterizerDesc, blendDesc);
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	GraphicsPipelineState(rootSignature[0], graphicsPipelineState[6], rasterizerDesc, blendDesc);
	GraphicsPipelineState(rootSignature[1], graphicsPipelineState[7], rasterizerDesc, blendDesc);

	
}

void SkinningConmmon::Blob(DirectXCommon* dxCommon, D3D12_ROOT_SIGNATURE_DESC descriptionSignature, Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature)
{
	HRESULT hr;
	// descriptionSignature が正しく設定されているか確認
	if (descriptionSignature.pParameters == nullptr || descriptionSignature.NumParameters == 0)
	{
		Logger::Log("descriptionSignature is not properly set.");
		assert(false);
		return;
	}
	//シリアライズにしてバイナリする
	Microsoft::WRL::ComPtr < ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr < ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));

		assert(false);
	}
	//バイナリを元に生成
	hr = dxCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));
}

void SkinningConmmon::GraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature, Microsoft::WRL::ComPtr<ID3D12PipelineState>& graphicsPipelineState, D3D12_RASTERIZER_DESC rasterizerDesc, D3D12_BLEND_DESC blendDesc)
{
	HRESULT hr;
	// InputLayout(インプットレイアウト)
	// VectorShaderへ渡す頂点データがどのようなものかを指定するオブジェクト


#pragma region InputLayout


	D3D12_INPUT_ELEMENT_DESC inputElementDescs[7] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].InputSlot = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[3].SemanticName = "TANGENT";
	inputElementDescs[3].SemanticIndex = 0;
	inputElementDescs[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[4].SemanticName = "BINORMAL";
	inputElementDescs[4].SemanticIndex = 0;
	inputElementDescs[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	inputElementDescs[5].SemanticName = "WEIGHT";
	inputElementDescs[5].SemanticIndex = 0;
	inputElementDescs[5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[5].InputSlot = 1;
	inputElementDescs[5].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs[6].SemanticName = "INDEX";
	inputElementDescs[6].SemanticIndex = 0;
	inputElementDescs[6].Format = DXGI_FORMAT_R32G32B32A32_SINT;
	inputElementDescs[6].InputSlot = 1;
	inputElementDescs[6].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;



	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);


#pragma endregion //InputLayout(インプットレイアウト)



	// Shaderをコンパイルする
	Microsoft::WRL::ComPtr < IDxcBlob> vertexShaderBlob = dxCommon_->CompileShader(L"resources/shaders/Skining/SkinningObject3d.VS.hlsl",
		L"vs_6_0");

	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr < IDxcBlob> pixelShaderBlob = dxCommon_->CompileShader(L"resources/shaders/Skining/SkinningObject3d.PS.hlsl",
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

	//利用するトロポジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

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

