#include "ParticleManager2d.h"
#include "DirectXGame/engine/base/Texture/TextureManager.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include"DirectXGame/engine/Manager/SRV/SrvManager.h"
#include"DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Material/Material.h"

#include "imgui.h"
#include <execution>
#undef max
#include <limits>
#include <windows.h>

void ParticleManager2d::Initialize(DirectXCommon* dxCommon, EffectManager* effectManager)
{
	dxCommon_ = dxCommon;			// DX共通クラス
	effectManager_ = effectManager;	// エフェクト管理クラス

	// SRV管理クラス
	srvManager_ = dxCommon_->GetSrvManager();
	
	// PSOマネージャー初期化
	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());

	// パイプライン生成
	CreateGraphicsPipeline();
}


// 更新
void ParticleManager2d::Update() {
	float deltaTime = MyGame::GameTime(); // 毎フレーム一定時間と仮定


	std::for_each(std::execution::par, particleGroups.begin(), particleGroups.end(),
		[&](auto& pair) {

			ParticleGroup2d& group = pair.second;
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


					// パーティクル影響
					ParticleFanction::Effect(group, particleIterator, deltaTime);

					// パーティクルデータをGPUに送る
					ParticleFanction::WorldDataForGPU(group, particleIterator, camera_);

					// 加算 
					++group.instanceCount;
				}

				++particleIterator;
			}
			group.material->GPUData();
		});
}
// 描画
void ParticleManager2d::Draw() {
	auto commandList = dxCommon_->GetCommandList();

	for (auto& pair : particleGroups) {
		ParticleGroup2d& group = pair.second;
		if (group.instanceCount == 0) {
			continue;
		}
		DrawCommonSetting();

		group.material->GetCommandListTexture(2);

		group.material->GetCommandListMaterial(0);

		group.sbParticleResource_.SetGraphicsRootDescriptorTable(1);

		group.mesh->GetCommandList();

		// インスタンシング描画
		uint32_t instanceCount = (std::min)(group.instanceCount, kNumMaxInstance);
		commandList->DrawIndexedInstanced(static_cast<UINT>(group.mesh->indices.size()), instanceCount, 0, 0, 0);
	}
}


void ParticleManager2d::CreateParticleGroup(const std::string name, const std::string textureFilePath, Sprite* sprite)
{
	// ランダムエンジンの初期化
	std::random_device seedGenerator;
	randomEngine_.seed(seedGenerator()); // randomEngine_ にシードを設定

	// あるなら
	if (particleGroups.Contains(name)) {
		return;
	}

	// パーティクルグループ生成
	ParticleFanction::Create(particleGroups[name], name, textureFilePath, kNumMaxInstance, dxCommon_, sprite);
}

void ParticleManager2d::CreateParticleGroup(const std::string name, const std::string textureFilePath, Primitive2D* primitive2d)
{

}

void ParticleManager2d::CreateParticleGroup(const std::string name, const std::string textureFilePath, ModelMesh* modelMesh)
{
	// ランダムエンジンの初期化
	std::random_device seedGenerator;
	randomEngine_.seed(seedGenerator()); // randomEngine_ にシードを設定

	// あるなら
	if (particleGroups.Contains(name)) {
		return;
	}

	// パーティクルグループ生成
	ParticleFanction::Create(particleGroups[name], name, textureFilePath, kNumMaxInstance, dxCommon_, modelMesh);
}

void ParticleManager2d::DrawCommonSetting() {
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());
	//// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
};


// ルートシグネチャの作成
void ParticleManager2d::CreateRootSignature() {
	D3D12_DESCRIPTOR_RANGE descriptorRange[2] = {};
	PSOFanction::SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ用
	PSOFanction::SetDescriptorRenge(descriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // インスタンシング用


	// RootParameter作成。複数指定できるのではい
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	// マテリアル (b0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// インスタンシング(t1) をバーテックシェーダ使用する
	PSOFanction::SetRootParameter(rootParameters[1], descriptorRange[1], D3D12_SHADER_VISIBILITY_VERTEX);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[2], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFanction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ


	// ルートシグネチャ作成
	psoManager_->SetRootSignature(rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));

}
// グラフィックスパイプラインの作成
void ParticleManager2d::CreateGraphicsPipeline() {
	CreateRootSignature();

	// DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);


	psoManager_->SetShaderFileName(ShaderFileName::VS, L"resources/shaders/Particle/2d/Particle2d.VS.hlsl");
	psoManager_->SetShaderFileName(ShaderFileName::PS, L"resources/shaders/Particle/2d/Particle2d.PS.hlsl");



	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
}


