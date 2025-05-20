#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
using namespace Microsoft::WRL;

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

template<class Type>
class ConstantBuffer {
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

	}

	void SetGraphicsRootConstantBufferView(int index)
	{
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(index, resource_->GetGPUVirtualAddress());
	}

	Type* Data() const { return data_; };
	
private:
	DirectXCommon* dxCommon_;

	Microsoft::WRL::ComPtr < ID3D12Resource> resource_;
	Type* data_;
};




