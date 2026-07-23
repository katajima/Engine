#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
#include<string>
using namespace Microsoft::WRL;

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

namespace Engine {
	/// <summary>
	/// 頂点バッファ
	/// </summary>
	/// <typeparam name="Type"></typeparam>
	template<class Type>
/// <summary>
/// VertexBufferを管理・実装するクラス。
/// </summary>
	class VertexBuffer {
	public:
		// 生成
		void CreateBufferView(DirectXCommon* dxCommon, std::vector<Type> vertex, size_t num = 1)
		{
			this->dxCommon = dxCommon;	// DX共通クラス

			/// <summary>
			/// リソース生成
			/// </summary>
			resource_ = dxCommon->GetDXGIDevice()->CreateBufferResource(sizeof(Type) * num);

			// データ
			data_ = nullptr;

			/// <summary>
			/// リソースを書き込むためのアドレス取得
			/// </summary>
			resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));

			std::memcpy(Data(), vertex.data(), sizeof(Type) * num);
			resource_->Unmap(0, nullptr);  // 🔧 安全なタイミングでアンマップ

			/// <summary>
			/// リソースの先頭のアドレスを作成する
			/// </summary>
			bufferView.BufferLocation = resource_->GetGPUVirtualAddress();
			bufferView.SizeInBytes = UINT(sizeof(Type) * num);
			bufferView.StrideInBytes = sizeof(Type);
		}

		/// <summary>
		/// 更新
		/// </summary>
		void UpdateBuffer(std::vector<Type> vertex) {
			if (resource_) {
				/// <summary>
				/// バッファサイズを確認
				/// </summary>
				size_t requiredSize = sizeof(Type) * vertex.size();
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
					bufferView.StrideInBytes = sizeof(Type);
				}

				// データのコピー
				Type* data;
				resource_->Map(0, nullptr, reinterpret_cast<void**>(&data));
				memcpy(data, vertex.data(), requiredSize);
				resource_->Unmap(0, nullptr);
			}
		}

		/// <summary>
		/// 一つのスロット
		/// </summary>
		void IASetVertexBuffers() {
			/// <summary>
			/// 頂点バッファの設定
			/// </summary>
			dxCommon->GetCommand()->GetList()->IASetVertexBuffers(0, 1, &bufferView);
		};

		/// <summary>
		/// 一つのスロット
		/// </summary>
		void IASetVertexBuffers(const D3D12_VERTEX_BUFFER_VIEW& vbv) {
			/// <summary>
			/// 頂点バッファの設定
			/// </summary>
			dxCommon->GetCommand()->GetList()->IASetVertexBuffers(0, 1, &vbv);
		};

		/// <summary>
		/// 二つのスロット
		/// </summary>
		void IASetVertexBuffersSlot(const D3D12_VERTEX_BUFFER_VIEW& vbv) {
			D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
			bufferView,
			vbv
			};
			/// <summary>
			/// 頂点バッファの設定
			/// </summary>
			dxCommon->GetCommand()->GetList()->IASetVertexBuffers(0, 2, vbvs);
		};

		// 二つのスロット
		void IASetVertexBuffersSlot(const D3D12_VERTEX_BUFFER_VIEW& vbv, const D3D12_VERTEX_BUFFER_VIEW& vbv2)
		{
			D3D12_VERTEX_BUFFER_VIEW vbvs[2] = {
			vbv,
			vbv2
			};

			/// <summary>
			/// 頂点バッファの設定
			/// </summary>
			dxCommon->GetCommand()->GetList()->IASetVertexBuffers(0, 2, vbvs);
		}

		/// <summary>
		/// データ取得
		/// </summary>
		Type* Data() const { return data_; };

		/// <summary>
		/// リソース取得
		/// </summary>
		Microsoft::WRL::ComPtr < ID3D12Resource> GetVertexResource() { return resource_; };
		/// <summary>
		/// デバッグレイヤーのLiveObject出力に頂点バッファ名を表示する
		/// </summary>
		void SetResourceName(const std::wstring& name) {
			if (resource_) {
				resource_->SetName(name.c_str());
			}
		}
		/// <summary>
		/// バッファビュー取得
		/// </summary>
		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return bufferView; }
	private:
		DirectXCommon* dxCommon = nullptr;

		Microsoft::WRL::ComPtr < ID3D12Resource> resource_;
		Type* data_ = nullptr;

		D3D12_VERTEX_BUFFER_VIEW bufferView{};

	};
}
