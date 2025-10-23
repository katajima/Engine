#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
using namespace Microsoft::WRL;

#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "DirectXGame/engine/DirectX/Barrier/Barrier.h"

/// <summary>
/// ストラクチャードバッファ
/// </summary>
/// <typeparam name="Type"></typeparam>
template<class Type>
class StructuredBuffer {
public:
	~StructuredBuffer() {
		if (data_) {
			resource_->Unmap(0, nullptr);
			data_ = nullptr;
		}
	}

	// 生成
	void CreateBuffer(DirectXCommon* dxCommon, int num = 1, bool useUav = false)
	{
		dxCommon_ = dxCommon;
		useUav_ = useUav;
		num_ = num;

		barrier_ = std::make_unique<Barrier>();
		barrier_->Initialize(dxCommon_->GetCommand());

		// リソース生成
		if (useUav_) {
			resource_ = dxCommon->GetDXGIDevice()->CreateBufferResourceUAV(sizeof(Type) * num_);
		}
		else {
			resource_ = dxCommon->GetDXGIDevice()->CreateBufferResource(sizeof(Type) * num_);

			// データ
			data_ = nullptr;

			// リソースを書き込むためのアドレス取得
			resource_->Map(0, nullptr, reinterpret_cast<void**>(&data_));
		}





		srvIndex_ = dxCommon_->GetSrvManager()->Allocate();
		srvHandleCPU_ = dxCommon_->GetSrvManager()->GetCPUDescriptorHandle(srvIndex_);
		srvHandleGPU_ = dxCommon_->GetSrvManager()->GetGPUDescriptorHandle(srvIndex_);
		dxCommon_->GetSrvManager()->CreateSRVforStructuredBuffer(srvIndex_, resource_.Get(), num_, sizeof(Type));

		if (useUav_) {
			uavIndex_ = dxCommon_->GetSrvManager()->Allocate();
			uavHandleCPU_ = dxCommon_->GetSrvManager()->GetCPUDescriptorHandle(uavIndex_);
			uavHandleGPU_ = dxCommon_->GetSrvManager()->GetGPUDescriptorHandle(uavIndex_);
			dxCommon_->GetSrvManager()->CreateUAVforStructuredBuffer(uavIndex_, resource_.Get(), num_, sizeof(Type));

			//
			barrier_->RegisterInitialState(resource_.Get(), D3D12_RESOURCE_STATE_COMMON);
		}
	}


	// グラフィックパイプラインにバッファを設定
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
	// コンピュートパイプラインにバッファを設定
	void SetComputeRootDescriptorTable(int index)
	{
		if (useUav_) {
			barrier_->TransitionResource(resource_.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
			dxCommon_->GetCommandList()->SetComputeRootDescriptorTable(index, uavHandleGPU_);
			barrier_->TransitionResource(resource_.Get(), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		}
		else {
			// SRV としてバインド（ComputeShader が t# で読み取る場合）
			dxCommon_->GetCommandList()->SetComputeRootDescriptorTable(index, srvHandleGPU_);
		}
	}
	// UAV依存
	void UavDependence() { barrier_->UavDependence(resource_.Get()); };

	// データ取得
	Type* Data() const {
		assert(!useUav_ && "UAVバッファにはCPUからアクセスできません");
		return data_; 
	};
	// リソース取得
	Microsoft::WRL::ComPtr < ID3D12Resource> GetResource() const { return resource_; }

	// StructuredBuffer クラス内に追加するメソッド
	void CopyFrom(const void* srcData, size_t byteSize)
	{
		assert(dxCommon_ && "StructuredBuffer::CopyFrom: dxCommon_ is null");
		assert(srcData && "StructuredBuffer::CopyFrom: srcData is null");
		size_t capacity = static_cast<size_t>(num_) * sizeof(Type);
		assert(byteSize <= capacity && "StructuredBuffer::CopyFrom: copy size exceeds buffer capacity");

		// CPUアクセス可能なバッファ（useUav_ == false）の場合は既に Map されている data_ に直接コピー
		if (!useUav_)
		{
			// data_ は CreateBuffer の時に Map してある前提
			memcpy(data_, srcData, byteSize);
			// 必要なら Flush/Invalidate を行う（UploadHeap では不要）
			return;
		}

		// ---------- UAV (DefaultHeap) 用バッファへのアップロード ----------
		// Upload 用バッファを作成してから CopyResource / CopyBufferRegion
		ComPtr<ID3D12Device> device = dxCommon_->GetDevice(); // dxCommon に GetDevice() を公開している想定
		ComPtr<ID3D12GraphicsCommandList> cmd = dxCommon_->GetCommandList(); //  コマンドリスト取得

		// Upload バッファを作成
		ComPtr<ID3D12Resource> uploadBuffer;
		{
			D3D12_HEAP_PROPERTIES heapProps = {};
			heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
			heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapProps.VisibleNodeMask = 1;
			heapProps.CreationNodeMask = 1;

			D3D12_RESOURCE_DESC resDesc = {};
			resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			resDesc.Alignment = 0;
			resDesc.Width = (UINT64)byteSize;
			resDesc.Height = 1;
			resDesc.DepthOrArraySize = 1;
			resDesc.MipLevels = 1;
			resDesc.Format = DXGI_FORMAT_UNKNOWN;
			resDesc.SampleDesc.Count = 1;
			resDesc.SampleDesc.Quality = 0;
			resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

			HRESULT hr = device->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&uploadBuffer)
			);
			assert(SUCCEEDED(hr));
		}

		// Map → memcpy → Unmap
		{
			uint8_t* mapped = nullptr;
			D3D12_RANGE range = { 0, 0 }; // CPU 書き込みのみなら読み取り範囲は0,0
			HRESULT hr = uploadBuffer->Map(0, &range, reinterpret_cast<void**>(&mapped));
			assert(SUCCEEDED(hr) && mapped);
			memcpy(mapped, srcData, byteSize);
			uploadBuffer->Unmap(0, nullptr);
		}

		// resource_ を COPY_DEST に遷移させてからコピー、コピー後に UAV (UNORDERED_ACCESS) に遷移
		// ※ barrier_ ヘルパーがある前提で使います（あなたの Barrier クラスの実装に合わせて調整して下さい）
		barrier_->TransitionResource(resource_.Get(), D3D12_RESOURCE_STATE_COPY_DEST);

		// Copy（サイズが同じならCopyResourceでも良いが CopyBufferRegion を使う）
		cmd->CopyBufferRegion(resource_.Get(), 0, uploadBuffer.Get(), 0, (UINT64)byteSize);

		// コピー完了後、UAV状態へ遷移しておく（ComputeShader が UAV として使う前に）
		barrier_->TransitionResource(resource_.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		// 必要に応じて UAV バリア（読み書きの順序保証）
		barrier_->UavDependence(resource_.Get());

		// uploadBuffer は関数終了で破棄（ComPtr が自動で release）
	}

	// コピー
	template<typename Container>
	void CopyFrom(const Container& c)
	{
		CopyFrom(static_cast<const void*>(c.data()), c.size() * sizeof(typename Container::value_type));
	}

	// vector<T> 便利オーバーロード
	void CopyFrom(const std::vector<Type>& v)
	{
		CopyFrom(static_cast<const void*>(v.data()), v.size() * sizeof(Type));
	}

private:
	DirectXCommon* dxCommon_ = nullptr;

	std::unique_ptr<Barrier> barrier_ = nullptr;	// バリア

	Microsoft::WRL::ComPtr < ID3D12Resource> resource_; // リソース

	//
	bool useUav_ = false;					// UAVを使用するかどうか
	int num_ = 1;

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




