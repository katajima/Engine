#include "PSOManager.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/DXCCompiler/DXCCompiler.h"

void Engine::PSOManager::Initialize(Command* command, DXGIDevice* DXGIDevice, DXCCompiler* dxcCompiler)
{
	this->command = command;
	this->dxgiDevice = DXGIDevice;
	this->dxcCompiler = dxcCompiler;
	useInputLayout_ = true;
}

void Engine::PSOManager::SetShederGraphics(D3D12_GRAPHICS_PIPELINE_STATE_DESC& graphicsPipeline)
{
	ShaderBlobSet shaderBlob{}; // コンパイル済みグラフィックスシェーダー一式


	if (shaderFiles_.vertex.filePath != L"") {
		// Shaderをコンパイルする
		shaderBlob.VS = dxcCompiler->CompileShader(shaderFiles_.vertex.filePath,
			L"vs_6_0");
		assert(shaderBlob.VS != nullptr);
		graphicsPipeline.VS = { shaderBlob.VS->GetBufferPointer(),
		shaderBlob.VS->GetBufferSize() }; // VertexShader
	}
	if (shaderFiles_.pixel.filePath != L"") {
		shaderBlob.PS = dxcCompiler->CompileShader(shaderFiles_.pixel.filePath,
			L"ps_6_0");
		assert(shaderBlob.PS != nullptr);
		graphicsPipeline.PS = { shaderBlob.PS->GetBufferPointer(),
		shaderBlob.PS->GetBufferSize() }; // PixelShader
	}
	if (shaderFiles_.domain.filePath != L"") {
		shaderBlob.DS = dxcCompiler->CompileShader(shaderFiles_.domain.filePath,
			L"ds_6_0");
		assert(shaderBlob.DS != nullptr);
		graphicsPipeline.DS = { shaderBlob.DS->GetBufferPointer(),
		shaderBlob.DS->GetBufferSize() }; // DomainShader
	}
	if (shaderFiles_.hull.filePath != L"") {
		shaderBlob.HS = dxcCompiler->CompileShader(shaderFiles_.hull.filePath,
			L"hs_6_0");
		assert(shaderBlob.HS != nullptr);
		graphicsPipeline.HS = { shaderBlob.HS->GetBufferPointer(),
		shaderBlob.HS->GetBufferSize() }; // HullShader
	}
	if (shaderFiles_.geometry.filePath != L"") {
		shaderBlob.GS = dxcCompiler->CompileShader(shaderFiles_.geometry.filePath,
			L"gs_6_0");
		assert(shaderBlob.GS != nullptr);
		graphicsPipeline.GS = { shaderBlob.GS->GetBufferPointer(),
		shaderBlob.GS->GetBufferSize() }; // GeometryShader
	}
	if (shaderFiles_.mesh.filePath != L"") {
	}
	if (shaderFiles_.amplification.filePath != L"") {
	}




}

void Engine::PSOManager::SetRootSignature(
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

	PSOFunction::Blob(dxgiDevice,descriptionSignature, rootSignature);
}

void Engine::PSOManager::GraphicsPipelineState(Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature, Microsoft::WRL::ComPtr<ID3D12PipelineState>& graphicsPipelineState, D3D12_BLEND_DESC blendDesc, D3D12_DEPTH_STENCIL_DESC depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType)
{
	HRESULT hr;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	if (useInputLayout_) {
		for (int i = 0; i < inputElementDesc_.size(); i++) {
			inputElementDesc_[i].SemanticName = semanticNames_[i].c_str();
			inputElementDesc_[i].Format = semanticformat_[i];
			inputElementDesc_[i].SemanticIndex = semanticIndex_[i];
			inputElementDesc_[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			inputElementDesc_[i].InputSlot = semanticSlot_[i];
		}

		
		inputLayoutDesc.pInputElementDescs = inputElementDesc_.data();
		inputLayoutDesc.NumElements = UINT(inputElementDesc_.size());
	}
	else {
		inputLayoutDesc.pInputElementDescs = nullptr;
		inputLayoutDesc.NumElements = 0;
	}





	// PSOを作成する

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();// RootSignature

	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;// InputLayout

	SetShederGraphics(graphicsPipelineStateDesc); // Sheder

	graphicsPipelineStateDesc.BlendState = blendDesc; //BlendState

	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;// RasterizerState

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = numRenderTargets_;
	if (numRenderTargets_ > 0) {
		graphicsPipelineStateDesc.RTVFormats[0] = rtvFormat_;
	}

	//利用するトロポジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = topologyType;

	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = dsvFormat_;

	
	hr = dxgiDevice->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));

	
	assert(SUCCEEDED(hr));
}

void Engine::PSOManager::AddInputElementDesc(const std::string& semanticName, UINT semanticIndex, DXGI_FORMAT format, UINT slot)
{
	// 文字列を vector に保持（ポインタが無効にならないようにする）
	semanticNames_.push_back(semanticName);
	semanticIndex_.push_back(semanticIndex);
	semanticformat_.push_back(format);
	semanticSlot_.push_back(slot);

	D3D12_INPUT_ELEMENT_DESC elementDesc{};
	inputElementDesc_.push_back(elementDesc);
}

void Engine::PSOManager::SetShaderFileName(ShaderFileName shaderFileName, std::wstring filename)
{
	switch (shaderFileName)
	{
	case ShaderFileName::PS:
		shaderFiles_.pixel.filePath = filename;
		break;
	case ShaderFileName::VS:
		shaderFiles_.vertex.filePath = filename;
		break;
	case ShaderFileName::DS:
		shaderFiles_.domain.filePath = filename;
		break;
	case ShaderFileName::HS:
		shaderFiles_.hull.filePath = filename;
		break;
	case ShaderFileName::GS:
		shaderFiles_.geometry.filePath = filename;
		break;
	case ShaderFileName::AS:
		shaderFiles_.amplification.filePath = filename;
		break;
	case ShaderFileName::MS:
		shaderFiles_.domain.filePath = filename;
		break;
	default:
		break;
	}


}

void Engine::PSOManager::SetDepthStencilDesc(bool flag, D3D12_DEPTH_WRITE_MASK mask, D3D12_COMPARISON_FUNC func)
{
	depthStencilDesc_ = {};

	// Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = flag;
	// 書き込みします
	depthStencilDesc_.DepthWriteMask = mask;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc_.DepthFunc = func;
}

void Engine::PSOManager::SetRasterizerDesc(D3D12_CULL_MODE cull, D3D12_FILL_MODE fill)
{
	//裏面(時計回り)を表示しない
	rasterizerDesc_.CullMode = cull;

	//三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = fill;
}

void Engine::PSOManager::SetRenderTargetFormats(UINT numRenderTargets, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat)
{
	numRenderTargets_ = numRenderTargets;
	rtvFormat_ = rtvFormat;
	dsvFormat_ = dsvFormat;
}

void Engine::PSOManager::DrawSetting(PSOType type,D3D12_PRIMITIVE_TOPOLOGY topology) {

	// 読み込み済みモデルを検索
	if (psoRoots_.contains(type)) {
		command->GetList()->SetPipelineState(psoRoots_[type].pipelineState.Get());
		// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		command->GetList()->SetGraphicsRootSignature(psoRoots_[type].rootSignature.Get());
		//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
		command->GetList()->IASetPrimitiveTopology(topology);
	}
}

void Engine::PSOManager::CreatePso(
	PSOType type, D3D12_ROOT_PARAMETER* rootParameter,UINT rootNum ,
	D3D12_STATIC_SAMPLER_DESC* samplerDesc,UINT samplerNum, 
	D3D12_CULL_MODE cull, D3D12_FILL_MODE fill, D3D12_BLEND_DESC blendDesc, D3D12_DEPTH_STENCIL_DESC depthStencilDesc,
	D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType)
{
	if (psoRoots_.contains(type)) {
		return;
	}

	SetRootSignature(psoRoots_[type].rootSignature, rootParameter, rootNum, samplerDesc, samplerNum);
	SetRasterizerDesc(cull, fill);
	GraphicsPipelineState(psoRoots_[type].rootSignature, psoRoots_[type].pipelineState, blendDesc, depthStencilDesc, topologyType);
}

