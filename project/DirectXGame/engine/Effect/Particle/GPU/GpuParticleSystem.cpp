#include "GpuParticleSystem.h"

#include"DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/DirectX/common/DirectXCommon.h"
#include "DirectXGame/engine/Light/LightCommon.h"


void GpuParticleGroup::Create(GpuParticleManager* gpuParticleManager, DirectXCommon* dxCommon, int MaxInstance, std::string name, std::string textureName)
{
	dxCommon_ = dxCommon;
	gpuParticleManager_ = gpuParticleManager;
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

	//	dxCommon_->GetCommandList()->Dispatch(UINT(dispatchCount), 1, 1);
	}

	sbTrailVertexResource_.UavDependence();
}

void GpuParticleGroup::UpdateEmitte(float deltaTime, int threadGroupCount)
{

	cbPerFrame_.Data()->time += deltaTime;
	cbPerFrame_.Data()->deltaTime = deltaTime;
	if (cbPerFrame_.Data()->time >= 60.0f) {
		cbPerFrame_.Data()->time = 0.0f;
	}




	sbParticleResource_.SetComputeRootDescriptorTable(0);		// パーティクル
	cbPerFrame_.SetComputeRootConstantBufferView(2);			// 乱数用時間
	sbFreeListIndexResource_.SetComputeRootDescriptorTable(3);	// カウンターインデックス
	sbFreeListResource_.SetComputeRootDescriptorTable(4);		// カウンター
	cbMaxInstance_.SetComputeRootConstantBufferView(5);			// Maxインスタンス


	// --- 重要: totalThreadCount は「総スレッド数」を入れる ---
	// threadGroupCount はグループ数。count_ がシェーダの THREAD_COUNT に相当する値である前提。
	emitterDispatchBuffer_.Data()->totalThreadCount = static_cast<uint32_t>(threadGroupCount * 64); // <-- 総スレッド数
	emitterDispatchBuffer_.SetComputeRootConstantBufferView(8);   // エミッタ発生数（CBVはデータ設定後にセット）

	// Dispatch にはグループ数を渡す
	dxCommon_->GetCommandList()->Dispatch(threadGroupCount, 1, 1);


	sbParticleResource_.UavDependence();
	sbFreeListIndexResource_.UavDependence();
	sbFreeListResource_.UavDependence();


}

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

void GpuParticleGroup::Draw() {

	if (mesh_) {
		sbParticleResource_.SetGraphicsRootDescriptorTable(1);

		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, dxCommon_->GetTextureManager()->GetSrvHandleGPU(textureName_));


		mesh_->GetCommandList();

		// インスタンシング描画
		dxCommon_->GetCommandList()->DrawIndexedInstanced(static_cast<UINT>(mesh_->indices.size()), cbMaxInstance_.Data()->maxInstance, 0, 0, 0);
	}
}