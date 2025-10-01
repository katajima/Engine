#include "GpuParticleSystem.h"

#include"DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/DirectX/common/DirectXCommon.h"
#include "DirectXGame/engine/Light/LightCommon.h"


void GpuParticleGroup::Create(GpuParticleManager* gpuParticleManager,DirectXCommon* dxCommon,int MaxInstance, std::string name, std::string textureName)
{
	dxCommon_ = dxCommon;
	gpuParticleManager_ = gpuParticleManager;
	// テクスチャ
	textureName_ = textureName;
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



	gpuParticleManager_->PreCsPso();
	sbParticleResource_.SetComputeRootDescriptorTable(0);		// パーティクル
	sbFreeListIndexResource_.SetComputeRootDescriptorTable(1);	// フリーリストインデックス
	sbFreeListResource_.SetComputeRootDescriptorTable(2);		// フリーリスト
	cbMaxInstance_.SetComputeRootConstantBufferView(3);			// Maxインスタンス

	{
		const uint32_t threadsPerGroup = 256;
		const uint32_t dispatchCount = (cbMaxInstance_.Data()->maxInstance + threadsPerGroup - 1) / threadsPerGroup;

		dxCommon_->GetCommandList()->Dispatch(UINT(dispatchCount), 1, 1);
	}



	


	// トレイル用最大値設定CB
	cbMaxTrailVertexInstance_.CreateBuffer(dxCommon_, 1);
	cbMaxTrailVertexInstance_.Data()->maxInstance = MaxInstance * 16;
	// トレイル頂点バッファ
	sbTrailVertexResource_.CreateBuffer(dxCommon_, cbMaxTrailVertexInstance_.Data()->maxInstance, true);
	// トレイル頂点カウンターインデックス
	sbTrailVertexFreeListIndexResource_.CreateBuffer(dxCommon_, 1, true);
	// トレイル頂点カウンター
	sbTrailVertexFreeListResource_.CreateBuffer(dxCommon_, cbMaxTrailVertexInstance_.Data()->maxInstance, true);

	gpuParticleManager_->PreCsTrailPso();
	sbTrailVertexResource_.SetComputeRootDescriptorTable(0);				// トレイル頂点
	sbTrailVertexFreeListIndexResource_.SetComputeRootDescriptorTable(1);	// フリーリストインデックス
	sbTrailVertexFreeListResource_.SetComputeRootDescriptorTable(2);		// フリーリスト
	cbMaxTrailVertexInstance_.SetComputeRootConstantBufferView(3);			// Maxインスタンス
	{
		const uint32_t threadsPerGroup = 256;
		const uint32_t dispatchCount = (cbMaxTrailVertexInstance_.Data()->maxInstance + threadsPerGroup - 1) / threadsPerGroup;

		dxCommon_->GetCommandList()->Dispatch(UINT(dispatchCount), 1, 1);
	}

	// メッシュ
	trailMesh_ = std::make_unique<TrailMesh>();
	trailMesh_->CreateMesh(cbMaxTrailVertexInstance_.Data()->maxInstance);
	trailMesh_->Initialize(dxCommon_);
}

void GpuParticleGroup::UpdateEmitte(float deltaTime,int count)
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


	
	dxCommon_->GetCommandList()->Dispatch(count, 1, 1);

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

	dxCommon_->GetCommandList()->Dispatch(UINT(dispatchCount), 1, 1);

	sbParticleResource_.UavDependence();
	sbFreeListIndexResource_.UavDependence();
	sbFreeListResource_.UavDependence();
}

void GpuParticleGroup::UpateTrailEmitte(float deltaTime)
{
	const uint32_t threadsPerGroup = 256;
	const uint32_t dispatchCount = (cbMaxInstance_.Data()->maxInstance + threadsPerGroup - 1) / threadsPerGroup;


	sbTrailVertexResource_.SetComputeRootDescriptorTable(0);				// トレイル
	sbTrailVertexFreeListIndexResource_.SetComputeRootDescriptorTable(1);	// カウンターインデックス(トレイル)
	sbTrailVertexFreeListResource_.SetComputeRootDescriptorTable(2);		// カウンター(トレイル)
	cbMaxTrailVertexInstance_.SetComputeRootConstantBufferView(3);			// Maxインスタンス(トレイル)


	sbParticleResource_.SetComputeRootDescriptorTable(4);					// パーティクル
	cbMaxInstance_.SetComputeRootConstantBufferView(5);						// Maxインスタンス(パーティクル)
	cbCameraPos_.SetComputeRootConstantBufferView(6);						// カメラ位置




	dxCommon_->GetCommandList()->Dispatch(UINT(dispatchCount), 1, 1);

	sbParticleResource_.UavDependence();
	sbTrailVertexResource_.UavDependence();
	sbTrailVertexFreeListIndexResource_.UavDependence();
	sbTrailVertexFreeListResource_.UavDependence();

}

void GpuParticleGroup::UpdateTrail()
{
	const uint32_t threadsPerGroup = 256;
	const uint32_t dispatchCount = (cbMaxTrailVertexInstance_.Data()->maxInstance + threadsPerGroup - 1) / threadsPerGroup;

	sbTrailVertexResource_.SetComputeRootDescriptorTable(0);				// トレイル
	sbTrailVertexFreeListIndexResource_.SetComputeRootDescriptorTable(1);	// カウンターインデックス(トレイル)
	sbTrailVertexFreeListResource_.SetComputeRootDescriptorTable(2);		// カウンター(トレイル)
	cbPerFrame_.SetComputeRootConstantBufferView(3);						// 乱数用時間
	cbMaxInstance_.SetComputeRootConstantBufferView(4);						// Maxインスタンス(パーティクル)

	dxCommon_->GetCommandList()->Dispatch(UINT(dispatchCount), 1, 1);


	sbTrailVertexResource_.UavDependence();
	sbTrailVertexFreeListIndexResource_.UavDependence();
	sbTrailVertexFreeListResource_.UavDependence();
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