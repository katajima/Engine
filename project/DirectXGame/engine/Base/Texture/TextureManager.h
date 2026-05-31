#pragma once
#include <windows.h>
#include<cstdint>
#include<string>
#include<fstream>
#include<sstream>
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
#include <filesystem>
#include <mutex>
using namespace Microsoft::WRL;
#include<vector>
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"
#include "DirectXGame/engine/Utility/TimerUtility.h"


namespace Engine {
	// 前方宣言
	class Command;
	class DXGIDevice;
	class SrvManager;
	// テクスチャマネージャー
	class TextureManager {
	public:
		TextureManager() = default;
		~TextureManager() = default;
		TextureManager(TextureManager&) = delete;
		TextureManager& operator=(TextureManager&) = delete;

		// 初期化
		void Initialize(Command* command, DXGIDevice* dxgiDevice, SrvManager* srvManager);


		//DirectTexを使ってTextureを読むためのLoadTextur関数
		void LoadTexture(const std::string& filePath);

		// 指定したディレクトリ内の全テクスチャを読み込む
		void LoadAllTexturesInDirectory(const std::string& directoryPath);

		// テクスチャ番号取得
		uint32_t GetTextureIndexByFilePath(const std::string& filePath);

		// テクスチャ番号からGPUハンドルを取得
		D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePach);

		// メタデータを取得
		const DirectX::TexMetadata& GetMataData(const std::string& filePach);

		// SRVマネージャー取得
		SrvManager* GetSrvManager() { return srvManager; }

		// ルートパラメーター設定
		static void SetRootParameter(D3D12_ROOT_PARAMETER& parameter, D3D12_DESCRIPTOR_RANGE& descriptorRange);

		// テクスチャリソース生成
		Microsoft::WRL::ComPtr <ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

		//データを転送するUploadTextureData関数を作る
		[[nodiscard]]
		Microsoft::WRL::ComPtr < ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr < ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

	private:


		//テクスチャ一枚分のデータ
		struct TextureData {
			DirectX::TexMetadata metadata;
			Microsoft::WRL::ComPtr<ID3D12Resource> resource;
			uint32_t srvIndex;
			D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
			D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
			Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;
		};



		//テクスチャデータ
		std::unordered_map<std::string, TextureData> textureDatas;
		mutable std::mutex textureMutex_;

		static uint32_t kSRVIndexTop;

		DebugTimer debugTimerTex_;
	private:
		Command* command = nullptr;
		DXGIDevice* dxgiDevice = nullptr;
		SrvManager* srvManager = nullptr;

	};
}
