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
using namespace Microsoft::WRL;
#include<vector>
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"



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
	void Initialize(Command* command,DXGIDevice* DXGIDevice, SrvManager* srvManager);
	

	//DirectTexを使ってTextureを読むためのLoadTextur関数
	void LoadTexture(const std::string& filePath);
	
	// テクスチャ番号取得
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);

	// テクスチャ番号からGPUハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePach);

	// メタデータを取得
	const DirectX::TexMetadata& GetMataData(const std::string& filePach);

	SrvManager* GetSrvManager(){ return srvManager_; }

	static void SetRootParameter(D3D12_ROOT_PARAMETER& parameter,D3D12_DESCRIPTOR_RANGE& descriptorRange);


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

	static uint32_t kSRVIndexTop;
	
	
private:
	Command* command_;
	DXGIDevice* DXGIDevice_;
	SrvManager* srvManager_ = nullptr;

};