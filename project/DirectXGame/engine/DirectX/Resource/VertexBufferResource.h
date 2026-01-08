#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
using namespace Microsoft::WRL;

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

namespace Engine {
	/// <summary>
	/// 頂点バッファ
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	template<class Type>
	class VertexBuffer {
	public:
		// 生成
		void CreateBufferView(DirectXCommon* dxCommon, std::vector<Type> vertex, size_t num = 1)
		{
			dxCommon_ = dxCommon;	// DX共通クラス

			// リソース生成
			resource_ = dxCommon->GetDXGIDevice()->CreateBufferResource(sizeof(Type) * num);

			// データ
			data_ = nullptr;

			// リソースを書き込むためのアドレス取得
			resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

			std::memcpy(Data(), vertex.data(), sizeof(Type) * num);
			resource_->Unmap(0, nullptr);  // 🔧 安全なタイミングでアンマップ

			// リソースの先頭のアドレスを作成する
			bufferView.BufferLocation = resource_->GetGPUVirtualAddress();
			bufferView.SizeInBytes = UINT(sizeof(Type) * num);
			bufferView.StrideInBytes = sizeof(Type);
		}

		// 更新
		void UpdateBuffer(std::vector<Type> vertex) {
			if (resource_) {
				// バッファサイズを確認
				size_t requiredSize = sizeof(Type) * vertex.size();
				D3D12_RESOURCE_DESC desc = resource_->GetDesc();
				if (requiredSize > desc.Width) {
					// バッファが不足している場合、再割り当て
					resource_.Reset();

					D3D12_HEAP_PROPERTIES heapProps = {};
					heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
					D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(requiredSize);
					HRESULT hr = dxCommon_->GetDXGIDevice()->GetDevice()->CreateCommittedResource(
						&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&resource_));

					if (FAILED(hr)) {
						// エラー処理
						return;
					}

					// バッファビューの更新
					bufferView.BufferLocation = resource_->GetGPUVirtualAddress();
					bufferView.SizeInBytes = UINT(requiredSize);
					bufferView.StrideInBytes = sizeof(Type);
				}

				// データのコピー
				Type* data;
				resource_->Map(0, nullptr, reinterpret_cast<void**>(&data));
				memcpy(data, vertex.data(), requiredSize);
				resource_->Unmap(0, nullptr);
			}
		}

		// 一つのスロット
		void IASetVertexBuffers() {
			// 頂点バッファの設定
			dxCommon_->GetCommand()->GetList()->IASetVertexBuffers(0, 1, &bufferView);
		};

		// 一つのスロット
		void IASetVertexBuffers(const D3D12_VERTEX_BUFFER_VIEW& vbv) {
			// 頂点バッファの設定
			dxCommon_->GetCommand()->GetList()->IASetVertexBuffers(0, 1, &vbv);
		};

		// 二つのスロット
		void IASetVertexBuffersSlot(const D3D12_VERTEX_BUFFER_VIEW& vbv) {
			D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
			bufferView,
			vbv
			};
			// 頂点バッファの設定
			dxCommon_->GetCommand()->GetList()->IASetVertexBuffers(0, 2, vbvs);
		};

		// 二つのスロット
		void IASetVertexBuffersSlot(const D3D12_VERTEX_BUFFER_VIEW& vbv, const D3D12_VERTEX_BUFFER_VIEW& vbv2)
		{
			D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
			vbv,
			vbv2
			};

			// 頂点バッファの設定
			dxCommon_->GetCommand()->GetList()->IASetVertexBuffers(0, 2, vbvs);
		}

		// データ取得
		Type* Data() const { return data_; };

		// リソース取得
		Microsoft::WRL::ComPtr < ID3D12Resource> GetVertexResource() { return resource_; };
		// バッファビュー取得
		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return bufferView; }
	private:
		DirectXCommon* dxCommon_;

		Microsoft::WRL::ComPtr < ID3D12Resource> resource_;
		Type* data_;

		D3D12_VERTEX_BUFFER_VIEW bufferView;

	};
}