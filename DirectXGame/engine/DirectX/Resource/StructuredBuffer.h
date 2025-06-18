#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
using namespace Microsoft::WRL;

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/DirectX/Barrier/Barrier.h"

template<class Type>
class StructuredBuffer {
public:

	void CreateBuffer(DirectXCommon* dxCommon, int num = 1, bool useUav = false)
	{
		dxCommon_ = dxCommon;
		useUav_ = useUav;

		barrier_ = std::make_unique<Barrier>();
		barrier_->Initialize(dxCommon_->GetCommand());

		// リソース生成
		if (useUav_) {
			resource_ = dxCommon->GetDXGIDevice()->CreateBufferResourceUAV(sizeof(Type) * num);
		}
		else {
			resource_ = dxCommon->GetDXGIDevice()->CreateBufferResource(sizeof(Type) * num);

			// データ
			data_ = nullptr;

			// リソースを書き込むためのアドレス取得
			resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
		}





		srvIndex_ = dxCommon_->GetSrvManager()->Allocate();
		srvHandleCPU_ = dxCommon_->GetSrvManager()->GetCPUDescriptorHandle(srvIndex_);
		srvHandleGPU_ = dxCommon_->GetSrvManager()->GetGPUDescriptorHandle(srvIndex_);
		dxCommon_->GetSrvManager()->CreateSRVforStructuredBuffer(srvIndex_, resource_.Get(), num, sizeof(Type));

		if (useUav_) {
			uavIndex_ = dxCommon_->GetSrvManager()->Allocate();
			uavHandleCPU_ = dxCommon_->GetSrvManager()->GetCPUDescriptorHandle(uavIndex_);
			uavHandleGPU_ = dxCommon_->GetSrvManager()->GetGPUDescriptorHandle(uavIndex_);
			dxCommon_->GetSrvManager()->CreateUAVforStructuredBuffer(uavIndex_, resource_.Get(), num, sizeof(Type));

			//
			barrier_->RegisterInitialState(resource_.Get(), D3D12_RESOURCE_STATE_COMMON);
		}
	}



	void SetGraphicsRootDescriptorTable(int index)
	{
		if (useUav_) {
			// UAV → SRV に切り替え
			barrier_->TransitionResource(resource_.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		}

		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(index, srvHandleGPU_);

		if (useUav_) {
			// SRV → UAV に戻す
			barrier_->TransitionResource(resource_.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		}
	}

	void SetComputeRootDescriptorTable(int index)
	{
		if (useUav_) {
			barrier_->TransitionResource(resource_.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
			dxCommon_->GetCommandList()->SetComputeRootDescriptorTable(index, uavHandleGPU_);
			barrier_->TransitionResource(resource_.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		}
	}

	void UavDependence() { barrier_->UavDependence(resource_.Get()); };

	Type* Data() const { return data_; };

	Microsoft::WRL::ComPtr < ID3D12Resource> GetResource() const { return resource_; }

private:
	DirectXCommon* dxCommon_ = nullptr;

	std::unique_ptr<Barrier> barrier_ = nullptr;	// バリア

	Microsoft::WRL::ComPtr < ID3D12Resource> resource_; // リソース

	//
	bool useUav_ = false;					// UAVを使用するかどうか


	// SRVハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;			// SRVハンドルGPU
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;			// SRVハンドルCPU
	uint32_t srvIndex_ = 0;							// インデックス



	// UAVハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE uavHandleGPU_;			// UAVハンドルGPU
	D3D12_CPU_DESCRIPTOR_HANDLE uavHandleCPU_;			// UAVハンドルCPU
	uint32_t uavIndex_ = 0;							// インデックス




	Type* data_;										// データ
};




