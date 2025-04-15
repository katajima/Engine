#include "TextureManager.h"
#include"StringUtility.h"
#include<unordered_map>

#include"DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"

//ImGuiで0番目を使用するため,1番から使用
uint32_t TextureManager::kSRVIndexTop = 1;

void TextureManager::Initialize(Command* command, DXGIDevice* DXGIDevice, SrvManager* srvManager)
{
    DXGIDevice_ = DXGIDevice;
    command_ = command;
    srvManager_ = srvManager;
    textureDatas.reserve(SrvManager::kMaxSRVCount);
}



void TextureManager::LoadTexture(const std::string& filePath) {
    // 読み込み済みテクスチャを検索
    if (textureDatas.contains(filePath)) {
        return;
    }
    // テクスチャ枚数上限チェック
    assert(srvManager_->IsMaxTextuer());

    // テクスチャファイルを読んでプログラムで扱えるようにする
    DirectX::ScratchImage image{};
    std::wstring filePathW = StringUtility::ConvertString(filePath);

    HRESULT hr;
    if (filePathW.ends_with(L".dds")) {
        hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
    }
    else {
        hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    }
   assert(SUCCEEDED(hr));

    // ミニマップ作成
    DirectX::ScratchImage mipImages{};
    if (DirectX::IsCompressed(image.GetMetadata().format)) { // 圧縮フォーマットかどうかを調べる
        mipImages = std::move(image);
    }
    else {
        hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 5, mipImages);
    }
   assert(SUCCEEDED(hr));

    // テクスチャデータを追加
    TextureData& textureData = textureDatas[filePath];
    textureData.metadata = mipImages.GetMetadata();
    textureData.resource = CreateTextureResource(textureData.metadata);
    textureData.intermediateResource = UploadTextureData(textureData.resource.Get(), mipImages);

    textureData.srvIndex = srvManager_->Allocate();
    textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
    textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);

    srvManager_->CreateSRVforTexture2D(textureData.srvIndex, textureData.resource.Get(), textureData.metadata);
}



uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath)
{
	//読み込み済みテクスチャを検索
    auto it = textureDatas.find(filePath);
    if (it != textureDatas.end()) {
        return it->second.srvIndex;
    }
	assert(0);
	return 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath)
{
    // 範囲外指定チェック
    auto it = textureDatas.find(filePath);
    if (it != textureDatas.end()) {
        return it->second.srvHandleGPU;
    }
    // エラー処理（例えば、ハンドルが見つからなかった場合）
    D3D12_GPU_DESCRIPTOR_HANDLE nullHandle = {};
    return nullHandle;
}

const DirectX::TexMetadata& TextureManager::GetMataData(const std::string& filePath)
{
    // 範囲外指定チェック
    auto it = textureDatas.find(filePath);
    assert(it != textureDatas.end() && "Texture file path not found");

    TextureData& textureData = it->second;
    return textureData.metadata;
}

void TextureManager::SetRootParameter(D3D12_ROOT_PARAMETER& parameter,D3D12_DESCRIPTOR_RANGE& descriptorRange)
{
    parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う           
    parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // PixelShaderで使う
    parameter.DescriptorTable.pDescriptorRanges = &descriptorRange; // Tableの中身の配列を指定
    parameter.DescriptorTable.NumDescriptorRanges = 1; // Tableで利用する数 
}

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResource(const DirectX::TexMetadata& metadata)
{
    //1. metadataを基にResourceの設定

    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = UINT(metadata.width); // Textureの幅
    resourceDesc.Height = UINT(metadata.height); // Textureの高さ
    resourceDesc.MipLevels = UINT16(metadata.mipLevels); // mipmapの数
    resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き or 配列Textureの配列数
    resourceDesc.Format = metadata.format; //TextureのFormat
    resourceDesc.SampleDesc.Count = 1; //サンプリングカウント。1固定。
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数。普段使っているのは2次元


    //2. 利用するHeapの設定

    //利用するHeapの設定。非常に特殊な運用。02_04exで一般的なケースがある
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; //細かい設定を行う
    //heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
    //heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; //プロセッサの近くに配置


    //3. Resourceを生成する

    Microsoft::WRL::ComPtr <ID3D12Resource> resource = nullptr;
    HRESULT hr = DXGIDevice_->GetDevice()->CreateCommittedResource(
        &heapProperties, // Heapの設定
        D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定。特になし
        &resourceDesc, // Resourceの設定
        D3D12_RESOURCE_STATE_COPY_DEST, // データ転送される設定
        nullptr, // Clear最適値。使わないのでnullptr
        IID_PPV_ARGS(&resource)); // 作成するResourceのポインタへのポインタ

    assert(SUCCEEDED(hr));

    return resource;
}

//データを転送するUploadTextureData関数を作る
[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages)
{
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload(DXGIDevice_->GetDevice(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
    uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresources.size()));
    Microsoft::WRL::ComPtr < ID3D12Resource> intermediateResource = DXGIDevice_->CreateBufferResource(intermediateSize);
    UpdateSubresources(command_->GetList(), texture.Get(), intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
    // Textureへの転送後は利用できるよう、D3D12_RESORCE_STATE_COPYからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
    D3D12_RESOURCE_BARRIER barrier{};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = texture.Get();
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    command_->GetList()->ResourceBarrier(1, &barrier);
    return intermediateResource;
}


