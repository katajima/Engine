#include "ParticleManager.h"
#include "DirectXGame/engine/base/Texture/TextureManager.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include"DirectXGame/engine/Manager/SRV/SrvManager.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/Effect/Primitive/Primitive.h"
#include"DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/Light/LightCommon.h"

#include "imgui.h"
#include <execution>
#undef max
#include <limits>
#include <windows.h>

void Engine::ParticleManager::Initialize(DirectXCommon* dxCommon, LightManager* lightManager, EffectManager* efectManager)
{
	this->dxCommon = dxCommon;							// DX共通クラス
	this->efectManager = efectManager;					// エフェクト管理クラス
	this->lineCommon = efectManager->GetLineCommon();	// ライン共通クラス
	this->srvManager = dxCommon->GetSrvManager();		// SRV管理クラス
	this->lightManager = lightManager;					// ライト管理クラス

	// PSOマネージャー初期化
	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon->GetCommand(), dxCommon->GetDXGIDevice(), dxCommon->GetDXCCompiler());
	
	// パイプライン生成
	CreateGraphicsPipeline();
}

void Engine::ParticleManager::DrawCommonSetting(EmitData::RasterizerType rasteType, EmitData::BlendType blendType, bool uvClamp)
{
	if (!uvClamp) {
		switch (blendType)
		{
		case EmitData::BlendType::MODE_ADD:
			if (rasteType == EmitData::RasterizerType::MODE_SOLID_BACK) {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[0].Get());
			}
			else {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[1].Get());
			}
			break;
		case EmitData::BlendType::MODE_SUBTRACT:
			if (rasteType == EmitData::RasterizerType::MODE_SOLID_BACK) {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[2].Get());
			}
			else {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[3].Get());
			}
			break;
		case EmitData::BlendType::MODE_MUlLIPLY:
			if (rasteType == EmitData::RasterizerType::MODE_SOLID_BACK) {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[4].Get());
			}
			else {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[5].Get());
			}
			break;
		default:
			break;
		}
		//// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	}
	else {

		switch (blendType)
		{
		case EmitData::BlendType::MODE_ADD:
			if (rasteType == EmitData::RasterizerType::MODE_SOLID_BACK) {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[6].Get());
			}
			else {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[7].Get());
			}
			break;
		case EmitData::BlendType::MODE_SUBTRACT:
			if (rasteType == EmitData::RasterizerType::MODE_SOLID_BACK) {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[8].Get());
			}
			else {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[9].Get());
			}
			break;
		case EmitData::BlendType::MODE_MUlLIPLY:
			if (rasteType == EmitData::RasterizerType::MODE_SOLID_BACK) {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[10].Get());
			}
			else {
				dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState[11].Get());
			}
			break;
		default:
			break;
		}
		//// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature2.Get());
	}

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Engine::ParticleManager::Update()
{
#ifdef _DEBUG
	ImGui::Begin("Field");
	for (auto& acc : fieldEffect_) {
		//	acc->DebugImgui();
	}
	ImGui::End();
#endif // _DEBUG

	float deltaTime = MyGame::GameTime(); // 毎フレーム一定時間と仮定

	std::for_each(std::execution::par, particleGroups.begin(), particleGroups.end(),
		[&](auto& pair) {

			ParticleGroup& group = pair.second;
			group.instanceCount = 0;



			// パーティクル粒子更新
			for (auto particleIterator = group.particle.begin(); particleIterator != group.particle.end(); ) {

				// ライフタイム
				if (particleIterator->lifeTime <= particleIterator->currentTime) {
					particleIterator = group.particle.erase(particleIterator);
					continue;
				}

				// 上限まで
				if (group.instanceCount < kNumMaxInstance) {

					// 場所による影響
					Field::Effect(group, particleIterator, fieldEffect_, deltaTime);

					// パーティクル影響
					ParticleFanction::Effect(group, particleIterator, deltaTime);

					// パーティクルデータをGPUに送る
					ParticleFanction::WorldDataForGPU(group, particleIterator, camera);

					// 加算 
					++group.instanceCount;
				}

				++particleIterator;
			}
			// マテリアルデータ
			ParticleFanction::MaterialEffect(group);
		});
}

void Engine::ParticleManager::Draw()
{
	auto commandList = dxCommon->GetCommandList();

	for (auto& pair : particleGroups) {
		ParticleGroup& group = pair.second;
		if (group.instanceCount == 0) {
			continue;
		}
		DrawCommonSetting(group.rasteType, group.blendType, group.isUVClamp);

		group.material->GetCommandListTexture(2);

		group.material->GetCommandListMaterial(0);

		lightManager->DrawLight({ true,false,false }, 3);

		group.sbParticleResource_.SetGraphicsRootDescriptorTable(1);

		group.mesh->GetCommandList();

		// インスタンシング描画
		uint32_t instanceCount = (std::min)(group.instanceCount, kNumMaxInstance);
		commandList->DrawIndexedInstanced(static_cast<UINT>(group.mesh->GetIndices().size()), instanceCount, 0, 0, 0);
	}
}

void Engine::ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath, Model* model, EmitData::RasterizerType rasteType, EmitData::BlendType blendType)
{
	debugTimer_.StartTimer(); // デバッグ用タイマー開始
	// ランダムエンジンの初期化
	std::random_device seedGenerator;
	randomEngine_.seed(seedGenerator()); // randomEngine_ にシードを設定

	// あるなら
	if (particleGroups.Contains(name)) {
		return;
	}

	// パーティクルグループ生成
	ParticleFanction::Create(particleGroups[name], name, textureFilePath, kNumMaxInstance, dxCommon, model->GetModelData().mesh[0].get(), rasteType, blendType);
	debugTimer_.EndTimer(); // デバッグ用タイマー終了
	debugTimer_.LogTimeSec("CreateParticleGroup ", " name");
}

void Engine::ParticleManager::CreateParticleGroup(const std::string name, const std::string textureFilePath, BasePrimitive* primitive, EmitData::RasterizerType rasteType, EmitData::BlendType blendType)
{
	debugTimer_.StartTimer(); // デバッグ用タイマー開始
	// ランダムエンジンの初期化
	std::random_device seedGenerator;
	randomEngine_.seed(seedGenerator()); // randomEngine_ にシードを設定

	// あるなら
	if (particleGroups.Contains(name)) {
		return;
	}

	// パーティクルグループ生成
	ParticleFanction::Create(particleGroups[name], name, textureFilePath, kNumMaxInstance, dxCommon, primitive->GetModelMesh(), rasteType, blendType);

	debugTimer_.EndTimer(); // デバッグ用タイマー終了
	debugTimer_.LogTimeSec("CreateParticleGroup ", " name");
}

#pragma region PSO

void Engine::ParticleManager::ClearParticle(std::string name)
{
	particleGroups[name].particle.clear();
}

void Engine::ParticleManager::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[2] = {};
	PSOFanction::SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ用
	PSOFanction::SetDescriptorRenge(descriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // インスタンシング用


	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	// マテリアル (b0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// インスタンシング(t1) をバーテックシェーダ使用する
	PSOFanction::SetRootParameter(rootParameters[1], descriptorRange[1], D3D12_SHADER_VISIBILITY_VERTEX);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[2], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	// 方向ライト (b1) をバーテックスシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[3], 1, D3D12_SHADER_VISIBILITY_VERTEX, D3D12_ROOT_PARAMETER_TYPE_CBV);


	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFanction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ


	// ルートシグネチャ作成
	psoManager_->SetRootSignature(rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));
	
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	
	psoManager_->SetRootSignature(rootSignature2, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));

}

void Engine::ParticleManager::CreateGraphicsPipeline()
{
	CreateRootSignature();

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 透明オブジェクトの場合はデプス書き込みを無効化
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	
	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	psoManager_->AddInputElementDesc("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);


	psoManager_->SetShaderFileName(ShaderFileName::VS, L"resources/shaders/Particle/CPU/Particle.VS.hlsl");
	psoManager_->SetShaderFileName(ShaderFileName::PS, L"resources/shaders/Particle/CPU/Particle.PS.hlsl");



	BlendAdd();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[0], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[1], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	BlendSubtract();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[2], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[3], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	BlendMuliply();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[4], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState[5], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);


	BlendAdd();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature2, graphicsPipelineState[6], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature2, graphicsPipelineState[7], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	BlendSubtract();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature2, graphicsPipelineState[8], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature2, graphicsPipelineState[9], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	BlendMuliply();
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature2, graphicsPipelineState[10], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature2, graphicsPipelineState[11], blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);




}

#pragma region Blend

void Engine::ParticleManager::BlendAdd()
{
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
}

void Engine::ParticleManager::BlendSubtract()
{
	// 減算ブレンドの設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// RGB の減算ブレンド
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;      // ソースの影響度
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE; // デスティネーションの影響度
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_SUBTRACT;     // 減算

	// アルファブレンドの設定（通常の設定）
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
}

void Engine::ParticleManager::BlendMuliply()
{

	// 加算ブレンドの設定
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// 通常のカラー（RGB）加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;      // ソースカラーをそのまま
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;     // デスティネーションカラーをそのまま
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;    // 加算（SrcColor + DestColor）
	// アルファブレンドの設定
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;


}

#pragma endregion // ブレンド

#pragma endregion // パイプライン関係