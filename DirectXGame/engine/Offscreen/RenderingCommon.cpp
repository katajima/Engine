#include "RenderingCommon.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/Camera/Camera.h"

void RenderingCommon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());

	// ビネット
	vignetteResource_ = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(VignetteGPU));
	vignetteResource_->Map(0, nullptr, reinterpret_cast<void**>(&vignetteData_));
	vignetteData_->scale = 16.0f;
	vignetteData_->squared = 0.8f;

	// スムージング
	smoothingResource_ = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(SmoothigGPU));
	smoothingResource_->Map(0, nullptr, reinterpret_cast<void**>(&smoothingData_));
	smoothingData_->num = 3;

	// ガウス
	gaussianResource_ = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(GaussianGPU));
	gaussianResource_->Map(0, nullptr, reinterpret_cast<void**>(&gaussianData_));
	gaussianData_->num = 3;
	gaussianData_->sigma = 2.0f;

	// アウトライン
	outlineResource_ = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(OutlineGPU));
	outlineResource_->Map(0, nullptr, reinterpret_cast<void**>(&outlineData_));
	outlineData_->num = 3;
	outlineData_->weightSquared = 6.0f;
	outlineData_->projectionInverse = Identity();

	// ラジアルブラー
	radialBlurResource_ = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(RadialBlurGPU));
	radialBlurResource_->Map(0, nullptr, reinterpret_cast<void**>(&radialBlurData_));
	radialBlurData_->center = Vector2{ 0.5f,0.5f };
	radialBlurData_->numSamples = 10;
	radialBlurData_->blurWidth = 0.01f;

	//	ディゾルブ
	dissovleResource_ = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(DissovleGPU));
	dissovleResource_->Map(0, nullptr, reinterpret_cast<void**>(&dissovleData_));
	dissovleData_->threshold = 0.5f;
	dissovleData_->edge = 0.03f;
	dissovleData_->color.x = 1.0f;
	dissovleData_->color.y = 0.4f;
	dissovleData_->color.z = 0.3f;


	//	ランダム
	randomResource_ = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(RandomGPU));
	randomResource_->Map(0, nullptr, reinterpret_cast<void**>(&randomData_));
	randomData_->time = 0.0f;

	//	ブルーム
	bloomResource_ = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(BloomGPU));
	bloomResource_->Map(0, nullptr, reinterpret_cast<void**>(&bloomData_));
	bloomData_->threshold = 0.9f;
	bloomData_->intensity = 1.0f;

	CreateGraphicsPipeline();



	vertexResource = dxCommon_->GetDXGIDevice()->CreateBufferResource(sizeof(ScreenVertexData) * 4);
	//リソースの先頭のアドレスを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つの分のサイズ
	vertexBufferView.SizeInBytes = sizeof(ScreenVertexData) * 4;
	//1頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(ScreenVertexData);


	dxCommon_->GetTextureManager()->LoadTexture("resources/Texture/noise.jpg");
	dissovleIndex = dxCommon_->GetTextureManager()->GetTextureIndexByFilePath("resources/Texture/noise.jpg");
}


#pragma region MyRegion

void RenderingCommon::DrawCopyRender(int index)
{
	DrawCopyImageSetting();

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);

	DrawColl();
}

void RenderingCommon::DrawGrayScaleRender(int index)
{
	DrawGrayScaleSetting();


	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);


	DrawColl();
}

void RenderingCommon::DrawSepiaeRender(int index)
{
	DrawSepiaSetting();

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);

	DrawColl();
}

void RenderingCommon::DrawVignetteRender(int index)
{
	DrawVignetteSetting();


	//vignetteData_->scale = 16.0f;
	// ビネットデータ
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, vignetteResource_->GetGPUVirtualAddress());

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);

	DrawColl();
}

void RenderingCommon::DrawSmoothingRender(int index)
{
	DrawSmoothingSetting();


	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, smoothingResource_->GetGPUVirtualAddress());

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);

	DrawColl();
}

void RenderingCommon::DrawGaussianRender(int index)
{
	DrawGaussianSetting();


	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, gaussianResource_->GetGPUVirtualAddress());

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);

	DrawColl();
}

void RenderingCommon::DrawOutlineRender(int index)
{
	DrawOutlineSetting();

	outlineData_->projectionInverse = Inverse(camera_->GetProjectionMatrix());
	outlineData_->nearZ = camera_->GetNearZ();
	outlineData_->farZ = camera_->GetFarZ();




	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, outlineResource_->GetGPUVirtualAddress());

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(2, dxCommon_->GetDepthStencil()->GetDepthSrvIndex());

	DrawColl();
}

void RenderingCommon::DrawRadialBlurRender(int index)
{
	DrawRadialBlurSetting();


	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, radialBlurResource_->GetGPUVirtualAddress());

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);

	DrawColl();
}

void RenderingCommon::DrawDissovleRender(int index)
{
	DrawDissovleSetting();


	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, dissovleResource_->GetGPUVirtualAddress());

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(2, dissovleIndex);

	DrawColl();
}

void RenderingCommon::DrawRandomRender(int index)
{
	DrawRandomSetting();

	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, randomResource_->GetGPUVirtualAddress());

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);

	DrawColl();
}

void RenderingCommon::DrawBloomRender(int index)
{
	DrawBloomSetting();

	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, bloomResource_->GetGPUVirtualAddress());

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);

	DrawColl();
}

void RenderingCommon::DrawBloomCombinRender(int index, int indexB)
{
	DrawBloomCombinSetting();

	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, bloomResource_->GetGPUVirtualAddress());

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(1, index);

	dxCommon_->GetSrvManager()->SetGraphicsRootdescriptorTable(2, indexB);

	DrawColl();
}

#pragma endregion // 描画


void RenderingCommon::UpdateImgui(PostEffectType type)
{
#ifdef _DEBUG
	if (ImGui::TreeNode("postEffect")) {
		switch (type)
		{
		case RenderingCommon::PostEffectType::kCopy:
			break;
		case RenderingCommon::PostEffectType::kGrayScale:
			break;
		case RenderingCommon::PostEffectType::kSepia:
			break;
		case RenderingCommon::PostEffectType::kVignette:
			ImGui::DragFloat("scale", &vignetteData_->scale, 0.01f);
			ImGui::DragFloat("squared", &vignetteData_->squared, 0.01f);
			break;
		case RenderingCommon::PostEffectType::kSmoothing:
			if (ImGui::Button("Filter3x3")) {
				smoothingData_->num = 3;
			}
			if (ImGui::Button("Filter5x5")) {
				smoothingData_->num = 5;
			}
			break;
		case RenderingCommon::PostEffectType::kGaussian:
			if (ImGui::Button("Filter3x3")) {
				gaussianData_->num = 3;
			}
			if (ImGui::Button("Filter5x5")) {
				gaussianData_->num = 5;
			}
			break;
		case RenderingCommon::PostEffectType::kOitline:
			if (ImGui::Button("0")) {
				outlineData_->num = 0;
			}
			if (ImGui::Button("1")) {
				outlineData_->num = 1;
			}
			if (ImGui::Button("2")) {
				outlineData_->num = 2;
			}
			if (ImGui::Button("3")) {
				outlineData_->num = 3;
			}
			ImGui::DragFloat("squared", &outlineData_->weightSquared, 0.1f);
			break;
		case RenderingCommon::PostEffectType::kRadialBlur:
			ImGui::DragFloat2("scale", &radialBlurData_->center.x, 0.01f);
			ImGui::DragFloat("blurWidth", &radialBlurData_->blurWidth, 0.01f);
			ImGui::SliderInt("numSamples", &radialBlurData_->numSamples, 1, 20);
			break;
		case RenderingCommon::PostEffectType::kDissovle:
			ImGui::DragFloat("threshold", &dissovleData_->threshold, 0.01f);
			ImGui::DragFloat("edge", &dissovleData_->edge, 0.001f);
			ImGui::ColorEdit3("color", &dissovleData_->color.x);
			break;
		case RenderingCommon::PostEffectType::kRandom:
			randomData_->time += 0.01f;
			break;
		case RenderingCommon::PostEffectType::kBloom:
			ImGui::DragFloat("threshold", &bloomData_->threshold, 0.001f);
			break;
		case RenderingCommon::PostEffectType::kBloomCombin:
			ImGui::DragFloat("intensity", &bloomData_->intensity, 0.01f);
			break;
		default:
			break;
		}
		ImGui::TreePop(); // <- 対応する TreePop を忘れずに！
	}
#endif // _DEBUG
}



#pragma region MyRegion

void RenderingCommon::DrawColl()
{
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView); //VBVを設定
	dxCommon_->GetCommandList()->DrawInstanced(3, 1, 0, 0);
}

void RenderingCommon::DrawCopyImageSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(copy_.rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(copy_.graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderingCommon::DrawGrayScaleSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(grayScale_.rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(grayScale_.graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderingCommon::DrawSepiaSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(sepia_.rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(sepia_.graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderingCommon::DrawVignetteSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(vignette_.rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(vignette_.graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderingCommon::DrawSmoothingSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(smoothing_.rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(smoothing_.graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderingCommon::DrawGaussianSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(gaussian_.rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(gaussian_.graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderingCommon::DrawOutlineSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(outline_.rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(outline_.graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderingCommon::DrawRadialBlurSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(radialBlur_.rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(radialBlur_.graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderingCommon::DrawDissovleSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(dissovle_.rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(dissovle_.graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderingCommon::DrawRandomSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(random_.rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(random_.graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderingCommon::DrawBloomSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(bloom_.rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(bloom_.graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderingCommon::DrawBloomCombinSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(bloomCombin_.rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(bloomCombin_.graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}




#pragma endregion // レンダリング種類

#pragma region MyRegion

void RenderingCommon::RootOutlineSetting()
{
	// アウトライン	
	D3D12_DESCRIPTOR_RANGE descriptorRangeOutline[2] = {};
	psoManager_->SetDescriptorRenge(descriptorRangeOutline[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ
	psoManager_->SetDescriptorRenge(descriptorRangeOutline[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // Depth用

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplersOutline[2] = {};
	psoManager_->SetSampler(staticSamplersOutline[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);
	psoManager_->SetSampler(staticSamplersOutline[1], 1, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_SHADER_VISIBILITY_PIXEL, PSOManager::TextureAddressMode::kCLAMP);

	D3D12_ROOT_PARAMETER outlineRootParameters[3] = {};

	psoManager_->SetRootParameter(outlineRootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(outlineRootParameters[1], descriptorRangeOutline[0], D3D12_SHADER_VISIBILITY_PIXEL);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(outlineRootParameters[2], descriptorRangeOutline[1], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(outline_.rootSignature, outlineRootParameters, _countof(outlineRootParameters), staticSamplersOutline, _countof(staticSamplersOutline));


}

void RenderingCommon::RootDissovleSetting()
{
	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	psoManager_->SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);


	// ディゾルブ
	D3D12_DESCRIPTOR_RANGE descriptorRangeDissovle[2] = {};
	psoManager_->SetDescriptorRenge(descriptorRangeDissovle[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ
	psoManager_->SetDescriptorRenge(descriptorRangeDissovle[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // Depth用

	D3D12_ROOT_PARAMETER dissovleRootParameters[3] = {};
	//　ディゾルブ(b0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(dissovleRootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(dissovleRootParameters[1], descriptorRangeDissovle[0], D3D12_SHADER_VISIBILITY_PIXEL);
	// テクスチャデータ (t1) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(dissovleRootParameters[2], descriptorRangeDissovle[1], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(dissovle_.rootSignature, dissovleRootParameters, _countof(dissovleRootParameters), staticSamplers, _countof(staticSamplers));

}

void RenderingCommon::RootRadialBlurSetting()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	psoManager_->SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplersBlur[1] = {};
	psoManager_->SetSampler(staticSamplersBlur[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL, PSOManager::TextureAddressMode::kCLAMP);

	D3D12_ROOT_PARAMETER RootParameters[2] = {};
	//ラジアルブラー (b0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(RootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(RootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(radialBlur_.rootSignature, RootParameters, _countof(RootParameters), staticSamplersBlur, _countof(staticSamplersBlur));

}

void RenderingCommon::RootBloomSetting()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	psoManager_->SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplersBlur[1] = {};
	psoManager_->SetSampler(staticSamplersBlur[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL, PSOManager::TextureAddressMode::kCLAMP);

	D3D12_ROOT_PARAMETER RootParameters[2] = {};
	// ブルーム (b0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(RootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(RootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(bloom_.rootSignature, RootParameters, _countof(RootParameters), staticSamplersBlur, _countof(staticSamplersBlur));


	D3D12_DESCRIPTOR_RANGE descriptorRangeCombin[2] = {};
	psoManager_->SetDescriptorRenge(descriptorRangeCombin[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);
	psoManager_->SetDescriptorRenge(descriptorRangeCombin[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	D3D12_ROOT_PARAMETER RootParametersCombin[3] = {};
	// ブルーム (b0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(RootParametersCombin[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(RootParametersCombin[1], descriptorRangeCombin[0], D3D12_SHADER_VISIBILITY_PIXEL);
	// テクスチャデータ (t1) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(RootParametersCombin[2], descriptorRangeCombin[1], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(bloomCombin_.rootSignature, RootParametersCombin, _countof(RootParametersCombin), staticSamplersBlur, _countof(staticSamplersBlur));



}


#pragma endregion // ルートパラメータやサンプラー設定



#pragma region MyRegion

void RenderingCommon::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	psoManager_->SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	psoManager_->SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);


	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	// マテリアルデータ (b0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	// ルートシグネチャ作成
	psoManager_->SetRootSignature(copy_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));

	psoManager_->SetRootSignature(grayScale_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));

	psoManager_->SetRootSignature(sepia_.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));


	// ビネット
	D3D12_ROOT_PARAMETER vinetteRootParameters[2] = {};
	// ビネットデータ (b0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(vinetteRootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(vinetteRootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(vignette_.rootSignature, vinetteRootParameters, _countof(vinetteRootParameters), staticSamplers, _countof(staticSamplers));


	// スムージング
	D3D12_ROOT_PARAMETER smoothingRootParameters[2] = {};
	// スムージングデータ (b0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(smoothingRootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(smoothingRootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(smoothing_.rootSignature, smoothingRootParameters, _countof(smoothingRootParameters), staticSamplers, _countof(staticSamplers));


	//ガウスデータ (b0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(smoothingRootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(smoothingRootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(gaussian_.rootSignature, smoothingRootParameters, _countof(smoothingRootParameters), staticSamplers, _countof(staticSamplers));




	// ランダム(b0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(smoothingRootParameters[0], 0, D3D12_SHADER_VISIBILITY_PIXEL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(smoothingRootParameters[1], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	psoManager_->SetRootSignature(random_.rootSignature, smoothingRootParameters, _countof(smoothingRootParameters), staticSamplers, _countof(staticSamplers));



	// ラジアルブラー
	RootRadialBlurSetting();

	// ディゾルブ
	RootDissovleSetting();

	// アウトライン
	RootOutlineSetting();

	// ブルーム
	RootBloomSetting();

}

void RenderingCommon::CreateGraphicsPipeline()
{
	CreateRootSignature();


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

	// InputLayout(インプットレイアウト)
	// VectorShaderへ渡す頂点データがどのようなものかを指定するオブジェクト

	// RasterizerState(ラスタライザステート)の設定
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);


	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT);


	//DepthStencilStateの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;


	psoManager_->shderFile_.vertex.filePach = L"resources/shaders/Offscreen/Fullscreen.VS.hlsl";



	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Offscreen/CopyImage.PS.hlsl";
	psoManager_->GraphicsPipelineState(copy_.rootSignature, copy_.graphicsPipelineState, blendDesc, depthStencilDesc);


	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Offscreen/GrayScale.PS.hlsl";
	psoManager_->GraphicsPipelineState(grayScale_.rootSignature, grayScale_.graphicsPipelineState, blendDesc, depthStencilDesc);


	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Offscreen/Sepia.PS.hlsl";
	psoManager_->GraphicsPipelineState(sepia_.rootSignature, sepia_.graphicsPipelineState, blendDesc, depthStencilDesc);


	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Offscreen/Vignette.PS.hlsl";
	psoManager_->GraphicsPipelineState(vignette_.rootSignature, vignette_.graphicsPipelineState, blendDesc, depthStencilDesc);

	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Offscreen/Smoothing.PS.hlsl";
	psoManager_->GraphicsPipelineState(smoothing_.rootSignature, smoothing_.graphicsPipelineState, blendDesc, depthStencilDesc);

	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Offscreen/Gaussian.PS.hlsl";
	psoManager_->GraphicsPipelineState(gaussian_.rootSignature, gaussian_.graphicsPipelineState, blendDesc, depthStencilDesc);

	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Offscreen/Outline.PS.hlsl";
	psoManager_->GraphicsPipelineState(outline_.rootSignature, outline_.graphicsPipelineState, blendDesc, depthStencilDesc);

	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Offscreen/RadialBlur.PS.hlsl";
	psoManager_->GraphicsPipelineState(radialBlur_.rootSignature, radialBlur_.graphicsPipelineState, blendDesc, depthStencilDesc);

	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Offscreen/Dissovle.PS.hlsl";
	psoManager_->GraphicsPipelineState(dissovle_.rootSignature, dissovle_.graphicsPipelineState, blendDesc, depthStencilDesc);

	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Offscreen/Random.PS.hlsl";
	psoManager_->GraphicsPipelineState(random_.rootSignature, random_.graphicsPipelineState, blendDesc, depthStencilDesc);

	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Offscreen/BloomExtract.PS.hlsl";
	psoManager_->GraphicsPipelineState(bloom_.rootSignature, bloom_.graphicsPipelineState, blendDesc, depthStencilDesc);

	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Offscreen/BloomCombine.PS.hlsl";
	psoManager_->GraphicsPipelineState(bloomCombin_.rootSignature, bloomCombin_.graphicsPipelineState, blendDesc, depthStencilDesc);



}

#pragma endregion // PSO
