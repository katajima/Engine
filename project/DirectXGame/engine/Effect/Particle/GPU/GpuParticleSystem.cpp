#include "GpuParticleSystem.h"

#include"DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/DirectX/common/DirectXCommon.h"
#include "DirectXGame/engine/Light/LightCommon.h"

enum RootIndex {
	ROOT_UAV_PARTICLE = 0,          // u0
	ROOT_CBV_PERFRAME = 1,          // b0
	ROOT_UAV_FREELIST_INDEX = 2,    // u1
	ROOT_UAV_FREELIST = 3,          // u2
	ROOT_CBV_MAXINSTANCE = 4,       // b1
	ROOT_SRV_EMITTERCOMMON = 5,     // t0
	ROOT_SRV_EMITTERTRAIL = 6,      // t1
	ROOT_SRV_EMITTERDISPATCH = 7,   // t2
	ROOT_CBV_DISPATCHCOUNT = 8      // b2
};

void GpuParticleGroup::Create(GpuParticleManager* gpuParticleManager, DirectXCommon* dxCommon, int MaxInstance, std::string name, std::string textureName)
{
	dxCommon_ = dxCommon;						// DX共通クラス
	gpuParticleManager_ = gpuParticleManager;	// GPUパーティクル管理クラス
	// テクスチャ
	textureName_ = textureName;
	trailTextureName_ = "resources/Texture/Image.png";

	// 名前
	name_ = name;

	// 最大値設定CB
	cbMaxInstance_.CreateBuffer(dxCommon_, 1);
	cbMaxInstance_.Data()->maxInstance = MaxInstance;
	// パーティクルインスタンシングVS
	sbParticleResource_.CreateBuffer(dxCommon_, cbMaxInstance_.Data()->maxInstance, true);
	// カウンターインデックス
	sbFreeListIndexResource_.CreateBuffer(dxCommon_, 1, true);
	// カウンター
	sbFreeListResource_.CreateBuffer(dxCommon_, cbMaxInstance_.Data()->maxInstance, true);
	// 時間
	cbPerFrame_.CreateBuffer(dxCommon_, 1);
	// カメラ位置
	cbCameraPos_.CreateBuffer(dxCommon_, 1);
	// エミッタ発生数
	emitterDispatchBuffer_.CreateBuffer(dxCommon_, 1);
	emitterDispatchBuffer_.Data()->totalThreadCount = 0; // 初期化
	// 削除フラグ
	cbDeleteParticleCS_.CreateBuffer(dxCommon_, 1);
	cbDeleteParticleCS_.Data()->isDelete = false; // 初期化


	gpuParticleManager_->PreCsPso();
	sbParticleResource_.SetComputeRootDescriptorTable(0);		// パーティクル
	sbFreeListIndexResource_.SetComputeRootDescriptorTable(1);	// フリーリストインデックス
	sbFreeListResource_.SetComputeRootDescriptorTable(2);		// フリーリスト
	cbMaxInstance_.SetComputeRootConstantBufferView(3);			// Maxインスタンス

	{
		const uint32_t threadsPerGroup = 256;
		const uint32_t dispatchCount = (cbMaxInstance_.Data()->maxInstance + threadsPerGroup - 1) / threadsPerGroup;

	//	dxCommon_->GetCommandList()->Dispatch(UINT(dispatchCount), 1, 1);
	}


	sbParticleResource_.UavDependence();
	sbFreeListIndexResource_.UavDependence();
	sbFreeListResource_.UavDependence();

	// CPU 側で作る最大バッファサイズ
	int maxSegments = 5; // 例: 1パーティクルあたり10セグメント
	int verticesPerSegment = 6; // Quadを三角形2個で作る
	int totalVertexCount = MaxInstance * maxSegments * verticesPerSegment;

	// トレイル用最大値設定CB
	cbMaxTrailVertexInstance_.CreateBuffer(dxCommon_, 1);
	cbMaxTrailVertexInstance_.Data()->maxInstance = totalVertexCount;
	// トレイル頂点バッファ
	sbTrailVertexResource_.CreateBuffer(dxCommon_, cbMaxTrailVertexInstance_.Data()->maxInstance, true);
	
	gpuParticleManager_->PreCsTrailPso();
	sbTrailVertexResource_.SetComputeRootDescriptorTable(0);				// トレイル頂点
	cbMaxTrailVertexInstance_.SetComputeRootConstantBufferView(1);			// Maxインスタンス
	{
		const uint32_t threadsPerGroup = 256;
		const uint32_t dispatchCount = (cbMaxTrailVertexInstance_.Data()->maxInstance + threadsPerGroup - 1) / threadsPerGroup;

	
	}

	sbTrailVertexResource_.UavDependence();


	emitterDispatchBuffer_.CreateBuffer(dxCommon_, 20);

	// ディスパッチカウント
	cbDispatchCount_.CreateBuffer(dxCommon_, 1);
	cbDispatchCount_.Data()->gEmitterDispatchCount = 0;


	// エミッター
	cbEmitterCommon_.CreateBuffer(dxCommon_, 20);
	cbEmitterCommon_.Data()->translate = Vector3(0.0f, 0.0f, 0.0f);
	cbEmitterCommon_.Data()->prevTranslate = cbEmitterCommon_.Data()->translate;
	cbEmitterCommon_.Data()->count = 10;
	cbEmitterCommon_.Data()->frequency = 0.00f;
	cbEmitterCommon_.Data()->frequencyTime = 0.0f;
	cbEmitterCommon_.Data()->emit = 0;
	cbEmitterCommon_.Data()->color = { 1.0f,0.8f ,0.1f };
	cbEmitterCommon_.Data()->colorRange = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.Data()->lifeTime = 5.0f;
	cbEmitterCommon_.Data()->velocity = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.Data()->velocityRange = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.Data()->scale = { 0.01f,0.01f,0.01f };
	cbEmitterCommon_.Data()->scaleRange = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.Data()->spawnShape = ParticleSpawnShape::Volume;
	cbEmitterCommon_.Data()->directionType = ParticleDireccion::Random;
	cbEmitterCommon_.Data()->force = 1.0f;
	cbEmitterCommon_.Data()->isAlhpa = true;
	cbEmitterCommon_.Data()->isScaling = false;
	cbEmitterCommon_.Data()->scaleAmount = 0.1f;
	cbEmitterCommon_.Data()->isGravity = false;
	cbEmitterCommon_.Data()->rotate = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.Data()->rotateRange = { 0.0f,0.0f,0.0f };
	cbEmitterCommon_.Data()->useBillboard = true;

	// トレイル
	cbEmitterTrail_.CreateBuffer(dxCommon_, 20);
	cbEmitterTrail_.Data()->trailcolor = { 1.0f,1.0f ,1.0f };
	cbEmitterTrail_.Data()->trailLifeTime = 0.5f;
	cbEmitterTrail_.Data()->isTrail = false;
	cbEmitterTrail_.Data()->trailWidth = 1.0f;

}

void GpuParticleGroup::Update()
{

	cbCameraPos_.Data()->x = camera_->transform_.translate.x;
	cbCameraPos_.Data()->y = camera_->transform_.translate.y;
	cbCameraPos_.Data()->z = camera_->transform_.translate.z;


	const uint32_t threadsPerGroup = 256;
	const uint32_t dispatchCount = (cbMaxInstance_.Data()->maxInstance + threadsPerGroup - 1) / threadsPerGroup;


	sbParticleResource_.SetComputeRootDescriptorTable(0);		// パーティクル
	cbPerFrame_.SetComputeRootConstantBufferView(1);			// 乱数用時間
	sbFreeListIndexResource_.SetComputeRootDescriptorTable(2);	// カウンターインデックス
	sbFreeListResource_.SetComputeRootDescriptorTable(3);		// カウンター
	cbMaxInstance_.SetComputeRootConstantBufferView(4);			// Maxインスタンス
	cbDeleteParticleCS_.SetComputeRootConstantBufferView(5); 	// 削除フラグ

	dxCommon_->GetCommandList()->Dispatch(UINT(dispatchCount), 1, 1);

	sbParticleResource_.UavDependence();
	sbFreeListIndexResource_.UavDependence();
	sbFreeListResource_.UavDependence();

	if (cbDeleteParticleCS_.Data()->isDelete != 0) {
		deleteTimer_ += cbPerFrame_.Data()->deltaTime;
		if(deleteTimer_ >= 0.05f){ // 0.5秒に一回削除
			deleteTimer_ = 0.0f;
			cbDeleteParticleCS_.Data()->isDelete = false; // フラグ戻す
		}
	}
}

void GpuParticleGroup::Draw() {

	if (mesh_) {
		sbParticleResource_.SetGraphicsRootDescriptorTable(1);

		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, dxCommon_->GetTextureManager()->GetSrvHandleGPU(textureName_));


		mesh_->GetCommandList();

		// インスタンシング描画
		dxCommon_->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(mesh_->indices.size()), cbMaxInstance_.Data()->maxInstance, 0, 0, 0);
	}
}

#pragma region Emit

void GpuParticleGroup::UpdateEmitte(float deltaTime)
{
	// time 更新（そのまま）
	cbPerFrame_.Data()->time += deltaTime;
	cbPerFrame_.Data()->deltaTime = deltaTime;
	if (cbPerFrame_.Data()->time >= 60.0f) cbPerFrame_.Data()->time = 0.0f;

	// --- 1) 各エミッタに particleStartOffset を割り振る（既にやっている） ---
	uint32_t particleOffset = 0;
	int emitterIndex = 0;
	// 事前に emitters を固定順 (vector 等) にしておくと emitterIndex が安定する
	for (auto& [id, emitter] : emitters)
	{
		auto& common = emitter->GetCommonData();
		common.particleStartOffset = particleOffset;
		particleOffset += common.particleMaxCount;
		++emitterIndex;
	}

	// --- 2) PerEmitterDispatch 配列を作成 ---
	std::vector<PerEmitterDispatch> dispatchList;
	dispatchList.reserve(emitters.size());

	uint32_t threadCursor = 0;      // 次に割り当てる startThread（スレッド単位）
	uint32_t particleCursor = 0;    // 次に割り当てる particleOffset（粒子インデックス単位）
	emitterIndex = 0;
	for (auto& [id, emitter] : emitters)
	{
		const auto& common = emitter->GetCommonData();

		// 1回の Emit で発生する数 = common.count
		// threadGroups (= グループ数) を求める
		uint32_t threadGroups = (common.count + threadCount - 1) / threadCount; // グループ数
		uint32_t threadsForEmitter = threadGroups * threadCount; // スレッド数（THREAD_COUNT の倍）

		PerEmitterDispatch info{};
		info.startThread = threadCursor;
		info.totalThreadCount = threadsForEmitter;
		info.particleOffset = particleCursor;
		info.emitterIndex = emitterIndex;

		dispatchList.push_back(info);

		threadCursor += threadsForEmitter;
		particleCursor += common.particleMaxCount;
		++emitterIndex;
	}

	// --- 3) 合計で何スレッド（グローバル）か計算して Dispatch 回数を求める ---
	uint32_t totalThreads = (dispatchList.empty() ? 0 : dispatchList.back().startThread + dispatchList.back().totalThreadCount);
	if (totalThreads == 0)
		return; // 発生するものがなければ早期 return

	uint32_t threadGroupCount = (totalThreads + threadCount - 1) / threadCount;

	// --- 4) GPU に PerEmitterDispatch 配列を転送（StructuredBuffer として） ---
	// sbEmitterDispatchResource_ は StructuredBuffer<PerEmitterDispatch> として用意しておく
	emitterDispatchBuffer_.CopyFrom(dispatchList.data(), dispatchList.size() * sizeof(PerEmitterDispatch));

	// --- 6) DispatchCount をセット（CBV） ---
	cbDispatchCount_.Data()->gEmitterDispatchCount = static_cast<uint32_t>(dispatchList.size());
	cbDispatchCount_.SetComputeRootConstantBufferView(ROOT_CBV_DISPATCHCOUNT);

	// --- 7) ルート/リソースをバインド（順序は RootSignature に合わせる） ---
	sbParticleResource_.SetComputeRootDescriptorTable(ROOT_UAV_PARTICLE);
	cbPerFrame_.SetComputeRootConstantBufferView(ROOT_CBV_PERFRAME);
	sbFreeListIndexResource_.SetComputeRootDescriptorTable(ROOT_UAV_FREELIST_INDEX);
	sbFreeListResource_.SetComputeRootDescriptorTable(ROOT_UAV_FREELIST);
	cbMaxInstance_.SetComputeRootConstantBufferView(ROOT_CBV_MAXINSTANCE);
	cbEmitterCommon_.SetComputeRootDescriptorTable(ROOT_SRV_EMITTERCOMMON);
	cbEmitterTrail_.SetComputeRootDescriptorTable(ROOT_SRV_EMITTERTRAIL);
	emitterDispatchBuffer_.SetComputeRootDescriptorTable(ROOT_SRV_EMITTERDISPATCH);

	// --- 7) DispatchCountCB（cbDispatchCount_） を Root にセット済み（上で） ---
	// --- 8) 実際に Dispatch ---
	dxCommon_->GetCommandList()->Dispatch(threadGroupCount, 1, 1);

	// --- 9) UAVバリア等（既存コード） ---
	sbParticleResource_.UavDependence();
	sbFreeListIndexResource_.UavDependence();
	sbFreeListResource_.UavDependence();
}



// エミッター追加
void GpuParticleGroup::AddEmitter(BaseGpuParticleEmitter* emit) {

	emitters[emit->GetName()] = emit;
	emitters[emit->GetName()]->GetCommonData();
};

#pragma endregion

#pragma region Field

void GpuParticleGroup::UpdateField()
{
	sbParticleResource_.SetComputeRootDescriptorTable(0);		// パーティクル
	cbPerFrame_.SetComputeRootConstantBufferView(1);			// 乱数用時間
	sbFreeListIndexResource_.SetComputeRootDescriptorTable(2);	// カウンターインデックス
	sbFreeListResource_.SetComputeRootDescriptorTable(3);		// カウンター
	cbMaxInstance_.SetComputeRootConstantBufferView(4);			// Maxインスタンス


	const uint32_t threadsPerGroup = 256;
	const uint32_t dispatchCount = (cbMaxInstance_.Data()->maxInstance + threadsPerGroup - 1) / threadsPerGroup;

	dxCommon_->GetCommandList()->Dispatch(UINT(dispatchCount), 1, 1);



	sbParticleResource_.UavDependence();
	sbFreeListIndexResource_.UavDependence();
	sbFreeListResource_.UavDependence();
}


#pragma endregion

#pragma region Trail

void GpuParticleGroup::UpateTrailEmitte(float deltaTime)
{
	const uint32_t threadsPerGroup = 256;
	const uint32_t dispatchCount = (cbMaxInstance_.Data()->maxInstance + threadsPerGroup - 1) / threadsPerGroup;


	sbTrailVertexResource_.SetComputeRootDescriptorTable(0);				// トレイル
	sbParticleResource_.SetComputeRootDescriptorTable(1);					// パーティクル
	cbMaxInstance_.SetComputeRootConstantBufferView(2);						// Maxインスタンス(パーティクル)
	cbCameraPos_.SetComputeRootConstantBufferView(3);						// カメラ位置




	dxCommon_->GetCommandList()->Dispatch(UINT(dispatchCount), 1, 1);

	sbParticleResource_.UavDependence();
	sbTrailVertexResource_.UavDependence();

}


void GpuParticleGroup::UpdateTrail()
{
	const uint32_t threadsPerGroup = 256;
	const uint32_t dispatchCount = (cbMaxTrailVertexInstance_.Data()->maxInstance + threadsPerGroup - 1) / threadsPerGroup;

	sbTrailVertexResource_.SetComputeRootDescriptorTable(0);				// トレイル
	cbPerFrame_.SetComputeRootConstantBufferView(1);						// 乱数用時間
	cbMaxTrailVertexInstance_.SetComputeRootConstantBufferView(2);			// Maxインスタンス(トレイル頂点)

	dxCommon_->GetCommandList()->Dispatch(UINT(dispatchCount), 1, 1);


	sbTrailVertexResource_.UavDependence();

}

void GpuParticleGroup::DrawTrail()
{

	sbTrailVertexResource_.SetGraphicsRootDescriptorTable(1);
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, dxCommon_->GetTextureManager()->GetSrvHandleGPU(trailTextureName_));
	D3D12_VERTEX_BUFFER_VIEW emptyVB = {};
	emptyVB.BufferLocation = 0;
	emptyVB.SizeInBytes = 0;
	emptyVB.StrideInBytes = 0;

	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &emptyVB);
	dxCommon_->GetCommandList()->IASetIndexBuffer(nullptr);
	// インスタンシング描画
	dxCommon_->GetCommandList()->DrawInstanced(static_cast<UINT>(cbMaxTrailVertexInstance_.Data()->maxInstance), 1, 0, 0);
}

#pragma endregion



