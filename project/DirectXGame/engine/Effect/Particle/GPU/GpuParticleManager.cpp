#include "GpuParticleManager.h"

#include"DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/DirectX/common/DirectXCommon.h"
#include "DirectXGame/engine/Light/LightCommon.h"


#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/Mesh/ModelMesh.h"

#include "DirectXgame/engine/MyGame/MyGame.h"
#include"DirectXGame/engine/Line/LineCommon.h"

void GpuParticleManager::Initialize(DirectXCommon* dxCommon, LightManager* lightManager, EffectManager* effectManager)
{
	effectManager_ = effectManager;
	srvManager_ = dxCommon->GetSrvManager();
	lineCommon_ = effectManager_->GetLineCommon();
	dxCommon_ = dxCommon;

	// パーティクルビュー
	cbPreViewResource_.CreateBuffer(dxCommon_, 1);


	// Compute用のパイプラインステートオブジェクトを作成(初期化)
	csPsoManager_ = std::make_unique<CSPSOManager>();
	csPsoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());

	// Compute用のパイプラインステートオブジェクトを作成(エミッター)

	csEmitPsoManagers_[EmitterType::AABB] = std::make_unique<CSPSOManager>();
	csEmitPsoManagers_[EmitterType::AABB]->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());

	csEmitPsoManagers_[EmitterType::Sphere] = std::make_unique<CSPSOManager>();
	csEmitPsoManagers_[EmitterType::Sphere]->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());

	csEmitPsoManagers_[EmitterType::Point] = std::make_unique<CSPSOManager>();
	csEmitPsoManagers_[EmitterType::Point]->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());


	// Compute用のパイプラインステートオブジェクトを作成(更新)
	csUpdatePsoManager_ = std::make_unique<CSPSOManager>();
	csUpdatePsoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());

	// Compute用のパイプラインステートオブジェクトを作成(影響場所)
	csFieldPsoManager_ = std::make_unique<CSPSOManager>();
	csFieldPsoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());

	// グラフィック用のパイプラインステートオブジェクトを作成
	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());



	// グラフィックパイプライン(トレイル用)
	psoTrailManager_ = std::make_unique<PSOManager>();
	psoTrailManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());
	// コンピュートパイプライン(トレイル初期化)
	csTrailInitPsoManager_ = std::make_unique<CSPSOManager>();
	csTrailInitPsoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());
	// コンピュートパイプライン(トレイルエミッタ)
	csTrailEmitPsoManager_ = std::make_unique<CSPSOManager>();
	csTrailEmitPsoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());
	// コンピュートパイプライン(トレイル更新)
	csTrailUpdatePsoManager_ = std::make_unique<CSPSOManager>();
	csTrailUpdatePsoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());


	// パイプライン作成
	CreateGraphicsPipeline();


	/// 初期化
	srvManager_->PreDraw();
}


void GpuParticleManager::Update()
{
	if (!camera_) return;

	Matrix4x4 cameraWorldMatrix = camera_->GetWorldMatrix();

	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, cameraWorldMatrix);
	Matrix4x4 billboardMatrixY = MakeBillboardMatrixY(billboardMatrix);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	cbPreViewResource_.Data()->billboardMatrix = billboardMatrix;
	cbPreViewResource_.Data()->billboardMatrixY = billboardMatrixY;
	cbPreViewResource_.Data()->viewProjection = camera_->GetViewProjectionMatrix();


	// パーティクルエミッター
	srvManager_->PreDraw();
	// エミッター更新
	for (auto& emitte : gpuParticleEmitter_) {
		csEmitPsoManagers_.find(emitte.second->GetType())->second->PreComputePSRS();
#ifdef _DEBUG
		ImGui::Begin("GPUEmit");
		emitte.second->UpdateImGui();
		ImGui::End();
#endif // _DEBUG

		emitte.second->Update(MyGame::GameTime());
	}

	csFieldPsoManager_->PreComputePSRS();


	// 影響場所更新
	for (auto& field : gpuParticleField_) {
#ifdef _DEBUG
		ImGui::Begin("GPUField");
		field.second.UpdateImgui();
		ImGui::End();
#endif // _DEBUG
		field.second.Update();
		for (auto& group : gpuParticleGroup_) {
			group.second.UpdateField();
		}
	}
	/// パーティクル更新
	csUpdatePsoManager_->PreComputePSRS();

	for (auto& group : gpuParticleGroup_) {
		group.second.SetCamera(camera_);
		group.second.Update();
	}


	csTrailEmitPsoManager_->PreComputePSRS();

	for (auto& group : gpuParticleGroup_) {
		group.second.UpateTrailEmitte(MyGame::GameTime());
	}

	csTrailUpdatePsoManager_->PreComputePSRS();

	for (auto& group : gpuParticleGroup_) {
		group.second.UpdateTrail();
	}

}

void GpuParticleManager::Draw()
{
	
	for (auto& group : gpuParticleGroup_) {

		// パーティクル描画

		dxCommon_->GetCommandList()->SetPipelineState(particleDraw.graphicsPipelineState.Get());
		//// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(particleDraw.rootSignature.Get());
		//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
		dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		
		cbPreViewResource_.SetGraphicsRootConstantBufferView(0);

		group.second.Draw();

		// トレイル描画

		dxCommon_->GetCommandList()->SetPipelineState(trailDraw.graphicsPipelineState.Get());
		//// RootSignatureを設定。PSOに設定しているけど別途設定が必要
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(trailDraw.rootSignature.Get());
		//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
		dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		group.second.DrawTrail();


	}

}


void GpuParticleManager::PreCsPso()
{
	csPsoManager_->PreComputePSRS();
}

void GpuParticleManager::PreCsTrailPso()
{
	csTrailInitPsoManager_->PreComputePSRS();
}

void GpuParticleManager::CreateGroup(std::string name, ModelMesh* mesh, std::string textureName, int instance)
{
	// あるなら
	if (gpuParticleGroup_.contains(name)) {
		return;
	}
	gpuParticleGroup_[name].Create(this, dxCommon_, instance, name, textureName);
	gpuParticleGroup_[name].SetMesh(mesh);
}

void GpuParticleManager::SetEmitteToGroup(std::string emitteName, std::string particleGroupName)
{
	// ないなら
	if (!gpuParticleGroup_.contains(particleGroupName)) {
		return;
	}
	// ないなら
	if (!gpuParticleEmitter_.contains(emitteName)) {
		return;
	}
	gpuParticleEmitter_[emitteName]->SetParticleGroup(&gpuParticleGroup_[particleGroupName]);
}

void GpuParticleManager::CreateField(std::string name)
{
	if (gpuParticleField_.contains(name)) {
		return;
	}

	gpuParticleField_[name].Init(dxCommon_, lineCommon_, name);

}

void GpuParticleManager::ClearEmitter(std::string nume)
{
	gpuParticleEmitter_.erase(nume);
}

void GpuParticleManager::ClearEmitterAll()
{
	gpuParticleEmitter_.clear();
}

void GpuParticleManager::ClearGroupParticle(std::string name)
{
	gpuParticleGroup_[name].Reset();
}

void GpuParticleManager::ClearGroupParticleAll()
{
	for (auto& group : gpuParticleGroup_) {
		group.second.Reset();
	}
}

void GpuParticleManager::ClearField(std::string name)
{
	gpuParticleField_.erase(name);
}

void GpuParticleManager::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[2] = {};
	PSOFanction::SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ用
	PSOFanction::SetDescriptorRenge(descriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // インスタンシング用


	// RootParameter作成。
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	// PerView (b0) を頂点シェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_VERTEX, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// パーティクルインスタンシング(t1) を頂点シェーダ使用する
	PSOFanction::SetRootParameter(rootParameters[1], descriptorRange[1], D3D12_SHADER_VISIBILITY_VERTEX);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	PSOFanction::SetRootParameter(rootParameters[2], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);



	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	PSOFanction::SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ

	// ルートシグネチャ作成
	psoManager_->SetRootSignature(particleDraw.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));



	// トレイルエフェクト用のルートシグネチャ作成
	psoTrailManager_->SetRootSignature(trailDraw.rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));

	//===============================
	// Compute用のPSOManagerを作成
	//===============================

	// 初期化
	{
		// Compute用のルートシグネチャを作成
		D3D12_DESCRIPTOR_RANGE computeDescriptorRange[3] = {};
		PSOFanction::SetDescriptorRenge(computeDescriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //Particle用
		PSOFanction::SetDescriptorRenge(computeDescriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //カウントインデックス用
		PSOFanction::SetDescriptorRenge(computeDescriptorRange[2], 2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //カウント用

		// Compute用のRootParameterを作成
		D3D12_ROOT_PARAMETER computeRootParameters[4] = {};
		PSOFanction::SetRootParameter(computeRootParameters[0], computeDescriptorRange[0], D3D12_SHADER_VISIBILITY_ALL);		// パーティクル
		PSOFanction::SetRootParameter(computeRootParameters[1], computeDescriptorRange[1], D3D12_SHADER_VISIBILITY_ALL);		// カウントインデックス
		PSOFanction::SetRootParameter(computeRootParameters[2], computeDescriptorRange[2], D3D12_SHADER_VISIBILITY_ALL);		// カウント
		PSOFanction::SetRootParameter(computeRootParameters[3], 0, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);	// 最大個数

		// Compute用のSamplerを設定
		csPsoManager_->SetRootSignature(computeRootParameters, _countof(computeRootParameters));
	}

	// エミッター
	{
		// Compute用のルートシグネチャを作成
		D3D12_DESCRIPTOR_RANGE computeDescriptorRange[3] = {};
		PSOFanction::SetDescriptorRenge(computeDescriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //Particle用
		PSOFanction::SetDescriptorRenge(computeDescriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //カウントインデックス用
		PSOFanction::SetDescriptorRenge(computeDescriptorRange[2], 2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //カウント用

		// Compute用のRootParameterを作成
		D3D12_ROOT_PARAMETER computeRootParameters[9] = {};
		PSOFanction::SetRootParameter(computeRootParameters[0], computeDescriptorRange[0], D3D12_SHADER_VISIBILITY_ALL);		// パーティクル
		PSOFanction::SetRootParameter(computeRootParameters[1], 0, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);	// エミッター(球)
		PSOFanction::SetRootParameter(computeRootParameters[2], 1, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);	// 乱数生成用時間
		PSOFanction::SetRootParameter(computeRootParameters[3], computeDescriptorRange[1], D3D12_SHADER_VISIBILITY_ALL);		// カウンターインデックス
		PSOFanction::SetRootParameter(computeRootParameters[4], computeDescriptorRange[2], D3D12_SHADER_VISIBILITY_ALL);		// カウンター
		PSOFanction::SetRootParameter(computeRootParameters[5], 2, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);	// 最大個数
		PSOFanction::SetRootParameter(computeRootParameters[6], 3, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);	// エミッター共通
		PSOFanction::SetRootParameter(computeRootParameters[7], 4, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);	// エミッタートレイル関係
		PSOFanction::SetRootParameter(computeRootParameters[8], 5, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);	// エミッターディスパッチ数

		// Compute用のSamplerを設定
		csEmitPsoManagers_[EmitterType::AABB]->SetRootSignature(computeRootParameters, _countof(computeRootParameters));
		csEmitPsoManagers_[EmitterType::Sphere]->SetRootSignature(computeRootParameters, _countof(computeRootParameters));
		csEmitPsoManagers_[EmitterType::Point]->SetRootSignature(computeRootParameters, _countof(computeRootParameters));
	}

	// 更新
	{
		// Compute用のルートシグネチャを作成
		D3D12_DESCRIPTOR_RANGE computeDescriptorRange[3] = {};
		PSOFanction::SetDescriptorRenge(computeDescriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //Particle用
		PSOFanction::SetDescriptorRenge(computeDescriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //カウントインデックス用
		PSOFanction::SetDescriptorRenge(computeDescriptorRange[2], 2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //カウント用

		// Compute用のRootParameterを作成
		D3D12_ROOT_PARAMETER computeRootParameters[6] = {};
		PSOFanction::SetRootParameter(computeRootParameters[0], computeDescriptorRange[0], D3D12_SHADER_VISIBILITY_ALL);			// パーティクル
		PSOFanction::SetRootParameter(computeRootParameters[1], 0, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);		// 乱数生成用時間
		PSOFanction::SetRootParameter(computeRootParameters[2], computeDescriptorRange[1], D3D12_SHADER_VISIBILITY_ALL);			// カウントインデックス
		PSOFanction::SetRootParameter(computeRootParameters[3], computeDescriptorRange[2], D3D12_SHADER_VISIBILITY_ALL);			// カウント
		PSOFanction::SetRootParameter(computeRootParameters[4], 1, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);		// 最大個数
		PSOFanction::SetRootParameter(computeRootParameters[5], 2, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);		// パーティクル削除

		// Compute用のSamplerを設定
		csUpdatePsoManager_->SetRootSignature(computeRootParameters, _countof(computeRootParameters));
	}

	// 場所影響
	{
		// Compute用のルートシグネチャを作成
		D3D12_DESCRIPTOR_RANGE computeDescriptorRange[3] = {};
		PSOFanction::SetDescriptorRenge(computeDescriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //Particle用
		PSOFanction::SetDescriptorRenge(computeDescriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //カウントインデックス用
		PSOFanction::SetDescriptorRenge(computeDescriptorRange[2], 2, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //カウント用

		// Compute用のRootParameterを作成
		D3D12_ROOT_PARAMETER computeRootParameters[6] = {};
		PSOFanction::SetRootParameter(computeRootParameters[0], computeDescriptorRange[0], D3D12_SHADER_VISIBILITY_ALL);			// パーティクル
		PSOFanction::SetRootParameter(computeRootParameters[1], 0, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);		// 乱数生成用時間
		PSOFanction::SetRootParameter(computeRootParameters[2], computeDescriptorRange[1], D3D12_SHADER_VISIBILITY_ALL);			// カウントインデックス
		PSOFanction::SetRootParameter(computeRootParameters[3], computeDescriptorRange[2], D3D12_SHADER_VISIBILITY_ALL);			// カウント
		PSOFanction::SetRootParameter(computeRootParameters[4], 1, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);		// 最大個数
		PSOFanction::SetRootParameter(computeRootParameters[5], 2, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);		// 影響Field

		// Compute用のSamplerを設定
		csFieldPsoManager_->SetRootSignature(computeRootParameters, _countof(computeRootParameters));
	}

	// トレイルエフェクト(初期化)
	{
		// Compute用のルートシグネチャを作成
		D3D12_DESCRIPTOR_RANGE computeDescriptorRange[1] = {};
		PSOFanction::SetDescriptorRenge(computeDescriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //頂点用
		
		// Compute用のRootParameterを作成
		D3D12_ROOT_PARAMETER computeRootParameters[2] = {};
		PSOFanction::SetRootParameter(computeRootParameters[0], computeDescriptorRange[0], D3D12_SHADER_VISIBILITY_ALL);		// トレイル頂点
		PSOFanction::SetRootParameter(computeRootParameters[1], 0, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);	// 最大頂点


		csTrailInitPsoManager_->SetRootSignature(computeRootParameters, _countof(computeRootParameters));
	}

	// トレイルエフェクト(エミッター)
	{
		// Compute用のルートシグネチャを作成
		D3D12_DESCRIPTOR_RANGE computeDescriptorRange[2] = {};
		PSOFanction::SetDescriptorRenge(computeDescriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //頂点用
		PSOFanction::SetDescriptorRenge(computeDescriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //パーティクル用
		

		// Compute用のRootParameterを作成
		D3D12_ROOT_PARAMETER computeRootParameters[4] = {};
		PSOFanction::SetRootParameter(computeRootParameters[0], computeDescriptorRange[0], D3D12_SHADER_VISIBILITY_ALL);		// トレイル頂点
		PSOFanction::SetRootParameter(computeRootParameters[1], computeDescriptorRange[1], D3D12_SHADER_VISIBILITY_ALL);		// パーティクル
		PSOFanction::SetRootParameter(computeRootParameters[2], 0, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);	// 最大頂点(パーティクル)
		PSOFanction::SetRootParameter(computeRootParameters[3], 1, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);	// カメラ位置

		csTrailEmitPsoManager_->SetRootSignature(computeRootParameters, _countof(computeRootParameters));
	}

	// トレイル(更新)
	{
		// Compute用のルートシグネチャを作成
		D3D12_DESCRIPTOR_RANGE computeDescriptorRange[1] = {};
		PSOFanction::SetDescriptorRenge(computeDescriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //頂点用
		
		// Compute用のRootParameterを作成
		D3D12_ROOT_PARAMETER computeRootParameters[3] = {};
		PSOFanction::SetRootParameter(computeRootParameters[0], computeDescriptorRange[0], D3D12_SHADER_VISIBILITY_ALL);		// トレイル頂点
		PSOFanction::SetRootParameter(computeRootParameters[1], 0, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);	// 乱数生成用時間
		PSOFanction::SetRootParameter(computeRootParameters[2], 1, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);	// 最大頂点(トレイル)
		
		csTrailUpdatePsoManager_->SetRootSignature(computeRootParameters, _countof(computeRootParameters));
	}

};


void GpuParticleManager::CreateGraphicsPipeline()
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

	psoManager_->SetShaderFileName(ShaderFileName::VS, L"resources/shaders/Particle/GPU/GpuParticle.VS.hlsl");
	psoManager_->SetShaderFileName(ShaderFileName::PS, L"resources/shaders/Particle/GPU/GpuParticle.PS.hlsl");


	// BlendState(ブレンドステート)の設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	//blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//blendDesc.RenderTarget[0].BlendEnable = TRUE;
	//blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	//blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	//blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(particleDraw.rootSignature, particleDraw.graphicsPipelineState, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);




	

	csPsoManager_->SetShaderFileName(L"resources/shaders/Particle/GPU/InitializeParticle.CS.hlsl");
	csPsoManager_->ComputePipelineState();

	csEmitPsoManagers_[EmitterType::AABB]->SetShaderFileName(L"resources/shaders/Particle/GPU/EmitParticleAABB.CS.hlsl");
	csEmitPsoManagers_[EmitterType::AABB]->ComputePipelineState();

	csEmitPsoManagers_[EmitterType::Sphere]->SetShaderFileName(L"resources/shaders/Particle/GPU/EmitParticleSphere.CS.hlsl");
	csEmitPsoManagers_[EmitterType::Sphere]->ComputePipelineState();
	
	csEmitPsoManagers_[EmitterType::Point]->SetShaderFileName(L"resources/shaders/Particle/GPU/EmitParticlePoint.CS.hlsl");
	csEmitPsoManagers_[EmitterType::Point]->ComputePipelineState();

	csUpdatePsoManager_->SetShaderFileName(L"resources/shaders/Particle/GPU/UpdateParticle.CS.hlsl");
	csUpdatePsoManager_->ComputePipelineState();

	csFieldPsoManager_->SetShaderFileName(L"resources/shaders/Particle/GPU/ParticleField.CS.hlsl");
	csFieldPsoManager_->ComputePipelineState();


	/// トレイルエフェクト用のパイプライン作成

	// トレイルPSO
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	psoTrailManager_->NoInputLayout();

	
	psoTrailManager_->SetShaderFileName(ShaderFileName::VS, L"resources/shaders/Particle/GPU/GpuParticleTrail.VS.hlsl");
	psoTrailManager_->SetShaderFileName(ShaderFileName::PS, L"resources/shaders/Particle/GPU/GpuParticleTrail.PS.hlsl");

	psoTrailManager_->SetRasterizerDesc(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	psoTrailManager_->GraphicsPipelineState(trailDraw.rootSignature, trailDraw.graphicsPipelineState, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);





	// トレイルエフェクト用のCSPSO
	csTrailInitPsoManager_->SetShaderFileName(L"resources/shaders/Particle/GPU/InitializeTrail.CS.hlsl");
	csTrailInitPsoManager_->ComputePipelineState();

	csTrailEmitPsoManager_->SetShaderFileName(L"resources/shaders/Particle/GPU/EmitTrail.CS.hlsl");
	csTrailEmitPsoManager_->ComputePipelineState();

	csTrailUpdatePsoManager_->SetShaderFileName(L"resources/shaders/Particle/GPU/UpdateTrail.CS.hlsl");
	csTrailUpdatePsoManager_->ComputePipelineState();

};