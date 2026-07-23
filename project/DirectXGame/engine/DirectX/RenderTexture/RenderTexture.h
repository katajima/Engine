#pragma once

// C++
#include <cmath>
#include <cassert>
#include <memory>
#include <wrl.h>
using namespace Microsoft::WRL;

// DirectX
#include <d3d12.h>
#include <dxgi1_6.h>

//#include"DirectXGame/engine/struct/Structs3D.h"

#include "DirectXGame/engine/Offscreen/PostEffectData.h"

namespace Engine {
	// 前方宣言
	class PostEffectData;
	class DXGIDevice;
	class Command;
	class SrvManager;
	class RtvManager;
	class RenderingCommon;
	class Camera;

	/// <summary>
	/// レンダーテクスチャクラス
	/// </summary>
	class RenderTexture
	{
	public:
		RenderTexture() = default;
		~RenderTexture();
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DXGIDevice* DXGIDevice, Command* command, SrvManager* srvManager, RtvManager* rvtManager, RenderingCommon* renderingCommonm, const std::string name, PostEffectType type);
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 描画
		/// </summary>
		void Draw();
		/// <summary>
		/// カメラ設定
		/// </summary>
		void SetCamera(Camera* camera) { this->camera = camera; }
		/// <summary>
		/// 他のSrvインデクス設定
		/// </summary>
		void SetOtherSrvIndex(uint32_t index) { otherSrvIndex_ = index; };


		/// <summary>
		/// SRVインデクス取得
		/// </summary>
		uint32_t GetSrvIndex() const { return srvIndex_; }
		/// <summary>
		/// クリアカラーを取得
		/// </summary>
		Vector4 GetClearColor()const;
		/// <summary>
		/// RTVハンドルを取得
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle();
		/// <summary>
		/// リソースを取得
		/// </summary>
		ID3D12Resource* GetResource();

		/// <summary>
		/// SRVGPUハンドル取得
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle();
		/// <summary>
		/// SRVCPUハンドル取得
		/// </summary>
		D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUHandle();
		/// <summary>
		/// ポストエフェクトデータ取得
		/// </summary>
		PostEffectData* GetPostEffectData();

	private:
		/// <summary>
		/// レンダーテクスチャのリソースを作成
		/// </summary>
		void CreateResource();
		// 
		/// <summary>
		/// レンダーテクスチャのリソースを作成
		/// </summary>
		void CreateResourcePixel();

		/// <summary>
		/// RTVを作成
		/// </summary>
		void CreateRTV();
		/// <summary>
		/// SRVを作成
		/// </summary>
		void CreateSRV();
	private:
		HRESULT hr_ = S_FALSE;
		// レンダーテクスチャ用リソース
		ComPtr<ID3D12Resource> resource_ = nullptr;
		// RTVインデックス
		uint32_t rtvIndex_ = 0;
		// SRVインデックス
		uint32_t srvIndex_ = 0;
		// クリアカラー(赤)
		const Vector4 clearColor_ = { 1.0f,0.0f,0.0f,1.0f };

		std::string name_;

		// もらってくるSRVインデックス
		uint32_t otherSrvIndex_ = 0;

	private:
		DXGIDevice* dxgiDevice = nullptr;
		Command* command = nullptr;
		SrvManager* srvManager = nullptr;
		RtvManager* rtvManager = nullptr;
		RenderingCommon* renderingCommon = nullptr;
		Camera* camera = nullptr;

		std::unique_ptr<PostEffectData> postEffectData_;

	public:
		PostEffectType type_;


	};
}
