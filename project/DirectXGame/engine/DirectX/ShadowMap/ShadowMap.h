#pragma once

#include <cstdint>
#include <memory>
#include <wrl.h>

#include <d3d12.h>

#include "DirectXGame/engine/struct/Matrix4x4.h"
#include "DirectXGame/engine/struct/Vector3.h"

namespace Engine {
	template<class Type>
	class ConstantBuffer;

	class DXGIDevice;
	class Command;
	class SrvManager;
	class Barrier;
	class DirectXCommon;
	class EntityManager;

/// <summary>
/// ShadowMapを管理・実装するクラス。
/// </summary>
	class ShadowMap {
	public:
		struct ShadowData {
			Matrix4x4 lightViewProjection;
			Vector3 lightDirection;
			float bias;
			float shadowMin;
			float pcfRadius;
			float padding0;
			float padding1;
		};

		struct Settings {
			float bias = 0.0025f;
			float shadowMin = 0.35f;
			float pcfRadius = 1.0f;
			float orthographicHalfSize = 80.0f;
			float lightDistance = 80.0f;
			float nearClip = 0.1f;
			float farClip = 200.0f;
		};

		~ShadowMap();

		void Initialize(DirectXCommon* dxCommon);
		void Update(EntityManager* entityManager);

		void PreDraw();
		void PostDraw();

		void SetGraphicsRootDescriptorTable(UINT rootParameterIndex);
		void SetGraphicsRootConstantBufferView(UINT rootParameterIndex);
		void RenderDebugImGui();

		ID3D12Resource* GetResource() const { return resource_.Get(); }
		D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const { return dsvHandle_; }
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle() const { return srvHandleGPU_; }
		const Matrix4x4& GetLightViewProjection() const;
		const Settings& GetSettings() const { return settings_; }
		Settings& GetSettings() { return settings_; }

		uint32_t GetWidth() const { return kShadowMapWidth; }
		uint32_t GetHeight() const { return kShadowMapHeight; }

	private:
		void CreateResource();
		void CreateDescriptors();
		Matrix4x4 CreateLightViewProjection(const Vector3& direction) const;
		void ApplySettingsToBuffer();
		void ClampSettings();
		void LoadSettings();
		void SaveSettings() const;

	private:
		static constexpr uint32_t kShadowMapWidth = 2048;
		static constexpr uint32_t kShadowMapHeight = 2048;
		static constexpr const char* kSettingsGroupName = "ShadowMap";
		static constexpr const char* kSettingsFilePath = "resources/GlobalVariables/ShadowMap.json";

		DXGIDevice* dxgiDevice_ = nullptr;
		Command* command_ = nullptr;
		SrvManager* srvManager_ = nullptr;
		Barrier* barrier_ = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_{};
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_{};
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_{};
		uint32_t srvIndex_ = 0;

		std::unique_ptr<ConstantBuffer<ShadowData>> shadowData_;
		Settings settings_{};
	};
}
