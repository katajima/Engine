#include "GpuParticleManager.h"

#include"DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/DirectX/common/DirectXCommon.h"
#include "DirectXGame/engine/Light/LightCommon.h"


#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/Mesh/ModelMesh.h"

#include "DirectXgame/engine/MyGame/MyGame.h"


void GpuParticleManager::Initialize(DirectXCommon* dxCommon, LightManager* lightManager, EffectManager* effectManager)
{
	effectManager_ = effectManager;
	srvManager_ = dxCommon->GetSrvManager();
	dxCommon_ = dxCommon;

	// パーティクルインスタンシングVS
	sbParticleResource_.CreateBuffer(dxCommon_, MaxInstance,true);
	
	// カウンター
	sbFreeCounterResource_.CreateBuffer(dxCommon_, 1, true);

	// パーティクルビュー
	preViewResource_ = effectManager_->GetDxCommon()->GetDXGIDevice()->CreateBufferResource(sizeof(PreView));
	preViewResource_->Map(0, nullptr, reinterpret_cast<void**>(&preView_));

	// 球エミッター
	cbEmitterSphere_.CreateBuffer(dxCommon_, 1);
	cbEmitterSphere_.Data()->count = 10;
	cbEmitterSphere_.Data()->frequency = 0.5f;
	cbEmitterSphere_.Data()->frequencyTime = 0.0f;
	cbEmitterSphere_.Data()->translate = Vector3(0.0f, 0.0f, 0.0f);
	cbEmitterSphere_.Data()->radius = 10.0f;
	cbEmitterSphere_.Data()->emit = 0;


	cbPerFrame_.CreateBuffer(dxCommon_, 1);
	




	textureName_ = "resources/Texture/Image.png";

	// Compute用のパイプラインステートオブジェクトを作成(初期化)
	csPsoManager_ = std::make_unique<CSPSOManager>();
	csPsoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());

	// Compute用のパイプラインステートオブジェクトを作成(エミッター)
	csEmitPsoManager_ = std::make_unique<CSPSOManager>();
	csEmitPsoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());

	// Compute用のパイプラインステートオブジェクトを作成(更新)
	csUpdatePsoManager_ = std::make_unique<CSPSOManager>();
	csUpdatePsoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());



	// グラフィック用のパイプラインステートオブジェクトを作成
	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_->GetCommand(), dxCommon_->GetDXGIDevice(), dxCommon_->GetDXCCompiler());

	// パイプライン作成
	CreateGraphicsPipeline();


	/// 初期化

	srvManager_->PreDraw();
	dxCommon_->GetCommandList()->SetComputeRootSignature(computeRootSignature.Get());
	dxCommon_->GetCommandList()->SetPipelineState(computePipelineState.Get());
	sbParticleResource_.SetComputeRootDescriptorTable(0);
	sbFreeCounterResource_.SetComputeRootDescriptorTable(1);
	dxCommon_->GetCommandList()->Dispatch(1, 1, 1);
}


void GpuParticleManager::Update()
{
	if (!camera_) return;

	Matrix4x4 cameraWorldMatrix = camera_->GetWorldMatrix();

	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, cameraWorldMatrix);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	preView_->billboardMatrix = billboardMatrix;

	preView_->viewProjection = camera_->GetViewProjectionMatrix();

	

	// 加算
	cbEmitterSphere_.Data()->frequencyTime += MyGame::GameTime();

	// 射出間隔を上回ったら射出許可を出して時間を調整
	if (cbEmitterSphere_.Data()->frequency <= cbEmitterSphere_.Data()->frequencyTime) {
		cbEmitterSphere_.Data()->frequencyTime -= cbEmitterSphere_.Data()->frequency;
		cbEmitterSphere_.Data()->emit = 1;
		
	}
	else {
		// 射出間隔を上回っていないので、射出許可は出せない
		cbEmitterSphere_.Data()->emit = 0;
	}
	
	cbPerFrame_.Data()->time += MyGame::GameTime();
	cbPerFrame_.Data()->deltaTime = MyGame::GameTime();


	srvManager_->PreDraw();
	dxCommon_->GetCommandList()->SetComputeRootSignature(computeEmitRootSignature.Get());
	dxCommon_->GetCommandList()->SetPipelineState(computeEmitPipelineState.Get());
	sbParticleResource_.SetComputeRootDescriptorTable(0);	// パーティクル
	cbEmitterSphere_.SetComputeRootConstantBufferView(1);	// エミッター
	cbPerFrame_.SetComputeRootConstantBufferView(2);		// 乱数用時間
	sbFreeCounterResource_.SetComputeRootDescriptorTable(3);// カウンター
	dxCommon_->GetCommandList()->Dispatch(1, 1, 1);

	sbParticleResource_.UavDependence();

	dxCommon_->GetCommandList()->SetComputeRootSignature(computeUpdateRootSignature.Get());
	dxCommon_->GetCommandList()->SetPipelineState(computeUpdatePipelineState.Get());
	sbParticleResource_.SetComputeRootDescriptorTable(0);	// パーティクル
	cbPerFrame_.SetComputeRootConstantBufferView(1);		// 乱数用時間

	dxCommon_->GetCommandList()->Dispatch(1, 1, 1);

}

void GpuParticleManager::Draw() 
{
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());
	//// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	if (mesh_) {
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, preViewResource_->GetGPUVirtualAddress());

		sbParticleResource_.SetGraphicsRootDescriptorTable(1);

		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, dxCommon_->GetTextureManager()->GetSrvHandleGPU(textureName_));


		mesh_->GetCommandList();

		// インスタンシング描画
		dxCommon_->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(mesh_->indices.size()), MaxInstance, 0, 0, 0);
		
	}
}


void GpuParticleManager::CreateRootSignature()
{
	D3D12_DESCRIPTOR_RANGE descriptorRange[2] = {};
	psoManager_->SetDescriptorRenge(descriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // テクスチャ用
	psoManager_->SetDescriptorRenge(descriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_SRV); // インスタンシング用
	

	// RootParameter作成。
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	// PerView (b0) を頂点シェーダで使用する
	psoManager_->SetRootParameter(rootParameters[0], 0, D3D12_SHADER_VISIBILITY_VERTEX, D3D12_ROOT_PARAMETER_TYPE_CBV);
	// パーティクルインスタンシング(t1) を頂点シェーダ使用する
	psoManager_->SetRootParameter(rootParameters[1], descriptorRange[1], D3D12_SHADER_VISIBILITY_VERTEX);
	// テクスチャデータ (t0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[2], descriptorRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	
	

	///Samplerの設定
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	psoManager_->SetSampler(staticSamplers[0], 0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_SHADER_VISIBILITY_PIXEL);// バイリニアフィルタ

	// ルートシグネチャ作成
	psoManager_->SetRootSignature(rootSignature, rootParameters, _countof(rootParameters), staticSamplers, _countof(staticSamplers));


	//===============================
	// Compute用のPSOManagerを作成
	//===============================

	{
		// Compute用のルートシグネチャを作成
		D3D12_DESCRIPTOR_RANGE computeDescriptorRange[2] = {};
		csPsoManager_->SetDescriptorRenge(computeDescriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //Particle用
		csPsoManager_->SetDescriptorRenge(computeDescriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //カウント用

		// Compute用のRootParameterを作成
		D3D12_ROOT_PARAMETER computeRootParameters[2] = {};
		csPsoManager_->SetRootParameter(computeRootParameters[0], computeDescriptorRange[0], D3D12_SHADER_VISIBILITY_ALL); // パーティクル
		csPsoManager_->SetRootParameter(computeRootParameters[1], computeDescriptorRange[1], D3D12_SHADER_VISIBILITY_ALL); // カウント

		// Compute用のSamplerを設定
		csPsoManager_->SetRootSignature(computeRootSignature, computeRootParameters, _countof(computeRootParameters), staticSamplers, _countof(staticSamplers));
	}

	{
		// Compute用のルートシグネチャを作成
		D3D12_DESCRIPTOR_RANGE computeDescriptorRange[2] = {};
		csEmitPsoManager_->SetDescriptorRenge(computeDescriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //Particle用
		csEmitPsoManager_->SetDescriptorRenge(computeDescriptorRange[1], 1, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //カウント用

		// Compute用のRootParameterを作成
		D3D12_ROOT_PARAMETER computeRootParameters[4] = {};
		csEmitPsoManager_->SetRootParameter(computeRootParameters[0], computeDescriptorRange[0], D3D12_SHADER_VISIBILITY_ALL);			// パーティクル
		csEmitPsoManager_->SetRootParameter(computeRootParameters[1], 0, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);	// エミッター(球)
		csEmitPsoManager_->SetRootParameter(computeRootParameters[2], 1, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);	// 乱数生成用時間
		csEmitPsoManager_->SetRootParameter(computeRootParameters[3], computeDescriptorRange[1], D3D12_SHADER_VISIBILITY_ALL);			// カウンター

		// Compute用のSamplerを設定
		csEmitPsoManager_->SetRootSignature(computeEmitRootSignature, computeRootParameters, _countof(computeRootParameters), staticSamplers, _countof(staticSamplers));
	}

	{
		// Compute用のルートシグネチャを作成
		D3D12_DESCRIPTOR_RANGE computeDescriptorRange[1] = {};
		csUpdatePsoManager_->SetDescriptorRenge(computeDescriptorRange[0], 0, 1, D3D12_DESCRIPTOR_RANGE_TYPE_UAV); //Particle用
		
		// Compute用のRootParameterを作成
		D3D12_ROOT_PARAMETER computeRootParameters[2] = {};
		csUpdatePsoManager_->SetRootParameter(computeRootParameters[0], computeDescriptorRange[0], D3D12_SHADER_VISIBILITY_ALL);			// パーティクル
		csUpdatePsoManager_->SetRootParameter(computeRootParameters[1], 0, D3D12_SHADER_VISIBILITY_ALL, D3D12_ROOT_PARAMETER_TYPE_CBV);		// 乱数生成用時間
		
		// Compute用のSamplerを設定
		csUpdatePsoManager_->SetRootSignature(computeUpdateRootSignature, computeRootParameters, _countof(computeRootParameters), staticSamplers, _countof(staticSamplers));
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


	psoManager_->shderFile_.vertex.filePach = L"resources/shaders/Particle/GPU/GpuParticle.VS.hlsl";
	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Particle/GPU/GpuParticle.PS.hlsl";


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


	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);
	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState, blendDesc, depthStencilDesc, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);




	csPsoManager_->shderFile_.commpute.filePach = L"resources/shaders/Particle/GPU/InitializeParticle.CS.hlsl";
	csPsoManager_->ComputePipelineState(computeRootSignature, computePipelineState);

	csEmitPsoManager_->shderFile_.commpute.filePach = L"resources/shaders/Particle/GPU/EmitParticle.CS.hlsl";
	csEmitPsoManager_->ComputePipelineState(computeEmitRootSignature, computeEmitPipelineState);

	csUpdatePsoManager_->shderFile_.commpute.filePach = L"resources/shaders/Particle/GPU/UpdateParticle.CS.hlsl";
	csUpdatePsoManager_->ComputePipelineState(computeUpdateRootSignature, computeUpdatePipelineState);


};