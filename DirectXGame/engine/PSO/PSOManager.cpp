#include "PSOManager.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/DXCCompiler/DXCCompiler.h"

void PSOManager::Initialize(Command* command, DXGIDevice* DXGIDevice, DXCCompiler* dxcCompiler)
{
	command_ = command;
	DXGIDevice_ = DXGIDevice;
	dxcCompiler_ = dxcCompiler;
}

void PSOManager::SetShederGraphics(D3D12_GRAPHICS_PIPELINE_STATE_DESC& graphicsPipeline)
{
	ShaderBlob shaderBlob{};


	if (shderFile_.vertex.filePach != L"") {
		// Shaderをコンパイルする
		shaderBlob.VS = dxcCompiler_->CompileShader(shderFile_.vertex.filePach,
			L"vs_6_0");
		assert(shaderBlob.VS != nullptr);
		graphicsPipeline.VS = { shaderBlob.VS->GetBufferPointer(),
		shaderBlob.VS->GetBufferSize() }; // VertexShader
	}
	if (shderFile_.pixel.filePach != L"") {
		shaderBlob.PS = dxcCompiler_->CompileShader(shderFile_.pixel.filePach,
			L"ps_6_0");
		assert(shaderBlob.PS != nullptr);
		graphicsPipeline.PS = { shaderBlob.PS->GetBufferPointer(),
		shaderBlob.PS->GetBufferSize() }; // PixelShader
	}
	if (shderFile_.domain.filePach != L"") {
		shaderBlob.DS = dxcCompiler_->CompileShader(shderFile_.domain.filePach,
			L"ds_6_0");
		assert(shaderBlob.DS != nullptr);
		graphicsPipeline.DS = { shaderBlob.DS->GetBufferPointer(),
		shaderBlob.DS->GetBufferSize() }; // DomainShader
	}
	if (shderFile_.hull.filePach != L"") {
		shaderBlob.HS = dxcCompiler_->CompileShader(shderFile_.hull.filePach,
			L"hs_6_0");
		assert(shaderBlob.HS != nullptr);
		graphicsPipeline.HS = { shaderBlob.HS->GetBufferPointer(),
		shaderBlob.HS->GetBufferSize() }; // HullShader
	}
	if (shderFile_.geometry.filePach != L"") {
		shaderBlob.GS = dxcCompiler_->CompileShader(shderFile_.geometry.filePach,
			L"gs_6_0");
		assert(shaderBlob.GS != nullptr);
		graphicsPipeline.GS = { shaderBlob.GS->GetBufferPointer(),
		shaderBlob.GS->GetBufferSize() }; // GeometryShader
	}
	if (shderFile_.mesh.filePach != L"") {
	}
	if (shderFile_.amplification.filePach != L"") {	
	}




}

void PSOManager::SetRootSignature(
	Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature,
	D3D12_ROOT_PARAMETER* rootParameter, UINT numRootParameters,
	D3D12_STATIC_SAMPLER_DESC* samplerDesc, UINT numSamplers)
{
	//psoRoot_.rootSignature = rootSignatu
	
	//Roosignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionSignature{};
	descriptionSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	descriptionSignature.pParameters = rootParameter;
	descriptionSignature.NumParameters = numRootParameters;
	descriptionSignature.pStaticSamplers = samplerDesc;
	descriptionSignature.NumStaticSamplers = numSamplers;

	PSOFanction::Blob(DXGIDevice_,descriptionSignature, rootSignature);
}

void PSOManager::GraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature, Microsoft::WRL::ComPtr<ID3D12PipelineState>& graphicsPipelineState, D3D12_BLEND_DESC blendDesc, D3D12_DEPTH_STENCIL_DESC depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType)
{
	HRESULT hr;

	for (int i = 0; i < inputElementDesc_.size(); i++) {
		inputElementDesc_[i].SemanticName = semanticNames_[i].c_str();
		inputElementDesc_[i].Format = semanticformat_[i];
		inputElementDesc_[i].SemanticIndex = semanticIndex_[i];
		inputElementDesc_[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDesc_[i].InputSlot = semanticSlot_[i];
	}

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDesc_.data();
	inputLayoutDesc.NumElements = UINT(inputElementDesc_.size());






	// PSOを作成する

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();// RootSignature

	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;// InputLayout

	SetShederGraphics(graphicsPipelineStateDesc); // Sheder

	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState

	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;// RasterizerState

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトロポジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = topologyType;

	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	
	hr = DXGIDevice_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));

	
	assert(SUCCEEDED(hr));
}

void PSOManager::AddInputElementDesc(const std::string& semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT slot)
{
	// 文字列を vector に保持（ポインタが無効にならないようにする）
	semanticNames_.push_back(semanticName);
	semanticIndex_.push_back(semanticIndex);
	semanticformat_.push_back(format);
	semanticSlot_.push_back(slot);

	D3D12_INPUT_ELEMENT_DESC elementDesc{};
	inputElementDesc_.push_back(elementDesc);
}

void PSOManager::SetDepthStencilDesc(bool flag, D3D12_DEPTH_WRITE_MASK mask, D3D12_COMPARISON_FUNC func)
{
	depthStencilDesc_ = {};

	// Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = flag;
	// 書き込みします
	depthStencilDesc_.DepthWriteMask = mask;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc_.DepthFunc = func;
}

void PSOManager::SetRasterizerDesc(D3D12_CULL_MODE cull, D3D12_FILL_MODE fill)
{
	//裏面(時計回り)を表示しない
	rasterizerDesc_.CullMode = cull;

	//三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = fill;
}

void PSOManager::DrawSetting(D3D12_PRIMITIVE_TOPOLOGY topology) {

	command_->GetList()->SetPipelineState(psoRoot_.graphicsPipelineState.Get());
	//// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	command_->GetList()->SetGraphicsRootSignature(psoRoot_.rootSignature.Get());
	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	command_->GetList()->IASetPrimitiveTopology(topology);
}

