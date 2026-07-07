#include "PSOFanction.h"

#include "assert.h"

#include "DirectXGame/engine/DirectX/DXCCompiler/DXCCompiler.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"

D3D12_BLEND_DESC Engine::PSOFanction::CreateAlphaBlendDesc() {
	// 描画先の全色成分へ、通常のアルファブレンドで書き込む設定を作成する
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	return blendDesc;
}

D3D12_DEPTH_STENCIL_DESC Engine::PSOFanction::CreateDepthStencilDesc() {
	// 手前または同じ深度のピクセルを描画し、深度値も更新する設定を作成する
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	return depthStencilDesc;
}

void Engine::PSOFanction::SetRootParameter(D3D12_ROOT_PARAMETER& parameter, int ShaderRegister, D3D12_SHADER_VISIBILITY shaderType, D3D12_ROOT_PARAMETER_TYPE rootType){
	// どのシェーダを使うのか
	parameter.ShaderVisibility = shaderType;

	// どのパラメータタイプを使うのか
	parameter.ParameterType = rootType;

	// レジスタ番号
	parameter.Descriptor.ShaderRegister = ShaderRegister;    // レジスタ番号0とバインド　　// b0の0と一致する。もしb11と紐づけたいなら11となる
}

void Engine::PSOFanction::SetRootParameter(D3D12_ROOT_PARAMETER& parameter, D3D12_DESCRIPTOR_RANGE& descriptorRange, D3D12_SHADER_VISIBILITY shaderType) {
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う           
	parameter.ShaderVisibility = shaderType;
	parameter.DescriptorTable.pDescriptorRanges = &descriptorRange; // Tableの中身の配列を指定
	parameter.DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数 	
};

void Engine::PSOFanction::SetDescriptorRenge(D3D12_DESCRIPTOR_RANGE& descriptorRange, int ShaderRegister, int numDescriptors, D3D12_DESCRIPTOR_RANGE_TYPE rengeType) {
	descriptorRange.BaseShaderRegister = ShaderRegister; //
	descriptorRange.RegisterSpace = 0; // レジスタスペース (通常は0) 
	descriptorRange.NumDescriptors = numDescriptors; // 数は1つ
	descriptorRange.RangeType = rengeType;
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // Offsetを自動計算
};

void Engine::PSOFanction::SetSampler(D3D12_STATIC_SAMPLER_DESC& staticSamplers, int shaderRegister, D3D12_FILTER filter, D3D12_SHADER_VISIBILITY shaderType, TextureAddressMode mode) {
	switch (mode)
	{
	case TextureAddressMode::kWRAP:
		staticSamplers.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 0～1の範囲外をリピート
		staticSamplers.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		break;
	case TextureAddressMode::kMIRROR:
		staticSamplers.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		staticSamplers.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		staticSamplers.AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		break;
	case TextureAddressMode::kCLAMP:
		staticSamplers.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		staticSamplers.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		staticSamplers.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		break;
	case TextureAddressMode::kBORDER:
		staticSamplers.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		staticSamplers.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		staticSamplers.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		break;
	case TextureAddressMode::kMIRROR_ONCE:
		staticSamplers.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
		staticSamplers.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
		staticSamplers.AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
		break;
	default:
		break;
	}

	staticSamplers.Filter = filter; // バイリニアフィルタ

	staticSamplers.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // 比較しない
	staticSamplers.MaxLOD = D3D12_FLOAT32_MAX; // ありったけのMipmapを使う
	staticSamplers.ShaderRegister = shaderRegister; //レジスタ番号0を使う
	staticSamplers.ShaderVisibility = shaderType; // PixelShaderで使う

};

void Engine::PSOFanction::Blob(DXGIDevice* DXGIDevice,D3D12_ROOT_SIGNATURE_DESC descriptionSignature, Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature) {
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
	hr = DXGIDevice->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

};
