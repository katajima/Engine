#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
using namespace Microsoft::WRL;

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

template<class Type>
class StructuredBuffer	 {
public:

	void CreateBuffer(DirectXCommon* dxCommon, int num = 1)
	{
		dxCommon_ = dxCommon;

		// リソース生成
		resource_ = dxCommon->GetDXGIDevice()->CreateBufferResource(sizeof(Type) * num);

		// データ
		data_ = nullptr;

		// リソースを書き込むためのアドレス取得
		resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

		

		index_ = dxCommon_->GetSrvManager()->Allocate();
		srvHandleCPU_ = dxCommon_->GetSrvManager()->GetCPUDescriptorHandle(index_);
		srvHandleGPU_ = dxCommon_->GetSrvManager()->GetGPUDescriptorHandle(index_);
		dxCommon_->GetSrvManager()->CreateSRVforStructuredBuffer(index_,resource_.Get(), num, sizeof(Type));
	}



	void SetGraphicsRootDescriptorTable(int index)
	{
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(index, srvHandleGPU_);
	}

	Type* Data() const { return data_; };

private:
	DirectXCommon* dxCommon_ = nullptr;

	Microsoft::WRL::ComPtr < ID3D12Resource> resource_; // リソース
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;			// SRVハンドルGPU
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;			// SRVハンドルCPU
	uint32_t index_	 = 0;								// インデックス

	Type* data_;										// データ
};




