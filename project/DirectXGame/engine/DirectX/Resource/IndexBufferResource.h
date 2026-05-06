#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
using namespace Microsoft::WRL;

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

namespace Engine {
	/// <summary>
	/// インデクスバッファ
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	template<class Type>
	class IndexBuffer {
	public:
		// 生成
		void CreateBufferView(DirectXCommon* dxCommon, std::vector<Type> index, size_t num = 1)
		{
			this->dxCommon = dxCommon;	// DX共通クラス

			// リソース生成
			resource_ = dxCommon->GetDXGIDevice()->CreateBufferResource(sizeof(Type) * num);

			// データ
			data_ = nullptr;

			// リソースを書き込むためのアドレス取得
			resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
			std::memcpy(Data(), index.data(), sizeof(Type) * num);
			resource_->Unmap(0, nullptr);
			// リソースの先頭のアドレスを作成する
			bufferView.BufferLocation = resource_->GetGPUVirtualAddress();
			bufferView.SizeInBytes = UINT(sizeof(Type) * index.size());
			bufferView.Format = DXGI_FORMAT_R32_UINT; // インデックスフォーマット
		}

		// 更新
		void UpdateBuffer(std::vector<Type> index) {
			if (resource_) {
				// バッファサイズを確認
				size_t requiredSize = sizeof(uint32_t) * index.size();
				D3D12_RESOURCE_DESC desc = resource_->GetDesc();
				if (requiredSize > desc.Width) {
					// バッファが不足している場合、再割り当て
					resource_.Reset();

					D3D12_HEAP_PROPERTIES heapProps = {};
					heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
					D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(requiredSize);
					HRESULT hr = dxCommon->GetDXGIDevice()->GetDevice()->CreateCommittedResource(
						&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource_));

					if (FAILED(hr)) {
						// エラー処理
						return;
					}

					// バッファビューの更新
					bufferView.BufferLocation = resource_->GetGPUVirtualAddress();
					bufferView.SizeInBytes = UINT(requiredSize);

				}
				// データのコピー
				Type* data;
				resource_->Map(0, nullptr, reinterpret_cast<void**>(&data));
				memcpy(data, index.data(), requiredSize);
				resource_->Unmap(0, nullptr);
			}
		}

		// 頂点バッファの設定
		void IASetIndexBuffer() {
			// 頂点バッファの設定
			dxCommon->GetCommand()->GetList()->IASetIndexBuffer(&bufferView);
		};



		// データ取得
		Type* Data() const { return data_; };
		// リソース取得
		Microsoft::WRL::ComPtr < ID3D12Resource> GetVertexResource() { return resource_; };
		// バッファビュー取得
		D3D12_INDEX_BUFFER_VIEW GetIndexxBufferView() { return bufferView; }
	private:
		DirectXCommon* dxCommon = nullptr;

		Microsoft::WRL::ComPtr < ID3D12Resource> resource_ = nullptr;
		Type* data_ = nullptr;

		D3D12_INDEX_BUFFER_VIEW bufferView{};

	};
}