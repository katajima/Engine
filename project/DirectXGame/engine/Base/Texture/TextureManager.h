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
/// <summary>
/// TextureManagerを管理・実装するクラス。
/// </summary>
	class TextureManager {
	public:
		TextureManager() = default;
		~TextureManager() = default;
		TextureManager(TextureManager&) = delete;
		TextureManager& operator=(TextureManager&) = delete;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Command* command, DXGIDevice* dxgiDevice, SrvManager* srvManager);


		/// <summary>
		/// DirectTexを使ってTextureを読むためのLoadTextur関数
		/// </summary>
		void LoadTexture(const std::string& filePath);

		/// <summary>
		/// 指定したディレクトリ内の全テクスチャを読み込む
		/// </summary>
		void LoadAllTexturesInDirectory(const std::string& directoryPath);

		/// <summary>
		/// テクスチャ番号取得
		/// </summary>
		uint32_t GetTextureIndexByFilePath(const std::string& filePath);

		/// <summary>
		/// 指定パスのテクスチャがロード済みかを確認
		/// </summary>
		bool HasTexture(const std::string& filePath) const;

		/// <summary>
		/// ImGuiの選択UIで使うロード済みテクスチャ一覧を取得
		/// </summary>
		std::vector<std::string> GetTextureFilePaths() const;

		/// <summary>
		/// テクスチャ番号からGPUハンドルを取得
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePach);

		/// <summary>
		/// メタデータを取得
		/// </summary>
		const DirectX::TexMetadata& GetMataData(const std::string& filePach);

		/// <summary>
		/// SRVマネージャー取得
		/// </summary>
		SrvManager* GetSrvManager() { return srvManager; }

		/// <summary>
		/// ルートパラメーター設定
		/// </summary>
		static void SetRootParameter(D3D12_ROOT_PARAMETER& parameter, D3D12_DESCRIPTOR_RANGE& descriptorRange);

		/// <summary>
		/// テクスチャリソース生成
		/// </summary>
		Microsoft::WRL::ComPtr <ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

		//データを転送するUploadTextureData関数を作る
		[[nodiscard]]
		Microsoft::WRL::ComPtr < ID3D12Resource> UploadTextureData(Microsoft::WRL::ComPtr < ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);

		/// <summary>
		/// GPU転送完了後に不要になるアップロード用中間リソースを解放
		/// </summary>
		void ReleaseIntermediateResources();

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
