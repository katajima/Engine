#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
#include<algorithm>
#include<string>
using namespace Microsoft::WRL;

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

namespace Engine {
	/// <summary>
	/// インデクスバッファ
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	template<class Type>
/// <summary>
/// IndexBufferを管理・実装するクラス。
/// </summary>
	class IndexBuffer {
	public:
		// 生成
		void CreateBufferView(DirectXCommon* dxCommon, std::vector<Type> index, size_t num = 1)
		{
			this->dxCommon = dxCommon;	// DX共通クラス
			const size_t bufferCount = (std::max)(size_t{ 1 }, num);

			/// <summary>
			/// リソース生成
			/// </summary>
			resource_ = dxCommon->GetDXGIDevice()->CreateBufferResource(sizeof(Type) * bufferCount);

			// データ
			data_ = nullptr;

			/// <summary>
			/// リソースを書き込むためのアドレス取得
			/// </summary>
			resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
			if (!index.empty()) {
				std::memcpy(Data(), index.data(), sizeof(Type) * index.size());
			}
			resource_->Unmap(0, nullptr);
			/// <summary>
			/// リソースの先頭のアドレスを作成する
			/// </summary>
			bufferView.BufferLocation = resource_->GetGPUVirtualAddress();
			bufferView.SizeInBytes = UINT(sizeof(Type) * index.size());
			bufferView.Format = DXGI_FORMAT_R32_UINT; // インデックスフォーマット
		}

		/// <summary>
		/// 更新
		/// </summary>
		void UpdateBuffer(std::vector<Type> index) {
			if (resource_) {
				/// <summary>
				/// バッファサイズを確認
				/// </summary>
				size_t requiredSize = sizeof(Type) * (std::max)(size_t{ 1 }, index.size());
				D3D12_RESOURCE_DESC desc = resource_->GetDesc();
				if (requiredSize > desc.Width) {
					/// <summary>
					/// バッファが不足している場合、再割り当て
					/// </summary>
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

					/// <summary>
					/// バッファビューの更新
					/// </summary>
					bufferView.BufferLocation = resource_->GetGPUVirtualAddress();
					bufferView.SizeInBytes = UINT(requiredSize);

				}
				// データのコピー
				Type* data;
				resource_->Map(0, nullptr, reinterpret_cast<void**>(&data));
				if (!index.empty()) {
					memcpy(data, index.data(), sizeof(Type) * index.size());
				}
				resource_->Unmap(0, nullptr);
			}
		}

		/// <summary>
		/// 頂点バッファの設定
		/// </summary>
		void IASetIndexBuffer() {
			/// <summary>
			/// 頂点バッファの設定
			/// </summary>
			dxCommon->GetCommand()->GetList()->IASetIndexBuffer(&bufferView);
		};



		/// <summary>
		/// データ取得
		/// </summary>
		Type* Data() const { return data_; };
		/// <summary>
		/// リソース取得
		/// </summary>
		Microsoft::WRL::ComPtr < ID3D12Resource> GetVertexResource() { return resource_; };
		/// <summary>
		/// デバッグレイヤーのLiveObject出力にインデックスバッファ名を表示する
		/// </summary>
		void SetResourceName(const std::wstring& name) {
			if (resource_) {
				resource_->SetName(name.c_str());
			}
		}
		/// <summary>
		/// バッファビュー取得
		/// </summary>
		D3D12_INDEX_BUFFER_VIEW GetIndexxBufferView() { return bufferView; }
	private:
		DirectXCommon* dxCommon = nullptr;

		Microsoft::WRL::ComPtr < ID3D12Resource> resource_ = nullptr;
		Type* data_ = nullptr;

		D3D12_INDEX_BUFFER_VIEW bufferView{};

	};
}
