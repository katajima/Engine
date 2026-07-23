#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
using namespace Microsoft::WRL;

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

/// <summary>
/// コンスタントバッファ
/// </summary>
/// <typeparam name="Type"></typeparam>
namespace Engine {
	template<class Type>
/// <summary>
/// ConstantBufferを管理・実装するクラス。
/// </summary>
	class ConstantBuffer {
	public:

		~ConstantBuffer() {
			if (resource_ && data_) {
				resource_->Unmap(0, nullptr);
				data_ = nullptr;
			}
			resource_.Reset(); // 明示的にComPtrも解放
		}
		//ConstantBuffer(const ConstantBuffer&) = delete;
		ConstantBuffer& operator=(const ConstantBuffer&) = delete;
		//生成
		void CreateBuffer(DirectXCommon* dxCommon, int num = 1)
		{
			this->dxCommon = dxCommon;

			// リソース生成
			resource_ = dxCommon->GetDXGIDevice()->CreateBufferResource(sizeof(Type) * num);

			// データ
			data_ = nullptr;

			// リソースを書き込むためのアドレス取得
			resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

		}
		// デバッグ名設定
		void SetResourceName(const wchar_t* name)
		{
			// LiveObject表示で定数バッファの残存元を追えるよう名前を付ける。
			if (resource_ && name) {
				resource_->SetName(name);
			}
		}
		// グラフィックパイプラインにバッファを設定
		void SetGraphicsRootConstantBufferView(int index)
		{
			dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(index, resource_->GetGPUVirtualAddress());
		}

		// コンピュートパイプラインにバッファを設定
		void SetComputeRootConstantBufferView(int index)
		{
			dxCommon->GetCommandList()->SetComputeRootConstantBufferView(index, resource_->GetGPUVirtualAddress());
		}

		// データ取得
		Type* Data() const { return data_; };

	private:
		DirectXCommon* dxCommon = nullptr;

		Microsoft::WRL::ComPtr < ID3D12Resource> resource_;
		Type* data_;
	};
}








