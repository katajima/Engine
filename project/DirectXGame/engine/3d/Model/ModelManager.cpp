#include "ModelManager.h"
#include "ModelAssetIdentity.h"
#include <stdexcept>
#include"ModelCommon.h"
#include"Model.h"
#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"


Engine::ModelManager::ModelManager(){}

Engine::ModelManager::~ModelManager()
{
	// DirectXCommonから明示Finalizeされなかった場合でもモデルを解放する
	Finalize();
}

void Engine::ModelManager::Initialize(DirectXCommon* dxCommon)
{
	this->dxCommon = dxCommon;	// DX共通クラス

	command = dxCommon->GetCommand();			// コマンド
	dxgiDevice = dxCommon->GetDXGIDevice();	// デバイス
	srvManager = dxCommon->GetSrvManager();	// SRV管理クラス

	// モデル共通クラス初期化
	modelCommon_ = std::make_unique<ModelCommon>();
	modelCommon_->Initialize(command, dxgiDevice, srvManager);
}

void Engine::ModelManager::Finalize()
{
	// 非同期読み込み中のモデルが残っている場合は、modelsを触る前に完了させる
	if (IsLoading()) {
		WaitAllLoadFinished();
	}

	{
		std::lock_guard<std::mutex> lock(modelsMutex_);
		// モデルが持つメッシュ、マテリアル、スキニング用D3D12リソースをここで解放する
		models.clear();
		loadingModelKeys_.clear();
	}

	// モデル共通クラスはモデル本体より後に解放する
	modelCommon_.reset();
	command = nullptr;
	dxgiDevice = nullptr;
	srvManager = nullptr;
	dxCommon = nullptr;
}

void Engine::ModelManager::LoadModel(const std::string& filePath, const std::string& dire)
{
	const auto key = ModelAssetIdentity::Normalize(filePath, dire);
	{
		std::lock_guard<std::mutex> lock(modelsMutex_);
		// 読み込み済みモデルを検索
		if (models.contains(key)) {
			return;
		}
	}

	//モデルの生成とファイル読み込み、初期化
	std::unique_ptr<Model> model = std::make_unique<Model>();
	const std::filesystem::path sourcePath(key);
    model->Initialize(dxCommon, modelCommon_.get(), sourcePath.parent_path().string(), sourcePath.filename().string(), "", &gpuResourceMutex_);

	{
		std::lock_guard<std::mutex> lock(modelsMutex_);
		// モデルをmapコンテナに格納
		models.insert(std::make_pair(key, std::move(model)));
	}
}

void Engine::ModelManager::LoadModelAsync(const std::string& filePath, const std::string& dire)
{
	const auto key = ModelAssetIdentity::Normalize(filePath, dire);
	// すでにロード済みかロックを使って確認
	{
		std::lock_guard<std::mutex> lock(modelsMutex_);
		if (models.contains(key) || loadingModelKeys_.contains(key)) {
			return;
		}
		loadingModelKeys_.insert(key);
	}

	// 非同期に読み込み開始
	std::future<void> future = std::async(std::launch::async, [this, filePath, dire, key]() {
		std::unique_ptr<Model> model = std::make_unique<Model>();
		// Assimp解析は各スレッドで進め、GPU共有リソースだけModel内部で短くロックする。
		const std::filesystem::path sourcePath(key);
        model->Initialize(dxCommon, modelCommon_.get(), sourcePath.parent_path().string(), sourcePath.filename().string(), "", &gpuResourceMutex_);

		{
			std::lock_guard<std::mutex> lock(modelsMutex_);
			models.insert(std::make_pair(key, std::move(model)));
			loadingModelKeys_.erase(key);
		}
		});

	std::lock_guard<std::mutex> lock(futuresMutex_);
	loadingFutures_.push_back(std::move(future));
}

void Engine::ModelManager::WaitAllLoadFinished()
{
	std::vector<std::future<void>> futures;
	{
		std::lock_guard<std::mutex> lock(futuresMutex_);
		futures.swap(loadingFutures_);
	}

	for (auto& fut : futures) {
		fut.get();  // 完了待ち
	}

	// 非同期ロード中に各スレッドが記録したGPU転送を、最後にまとめて実行して完了を待つ。
	std::lock_guard<std::mutex> lock(gpuResourceMutex_);
	auto* command = dxCommon->GetCommand();
	command->KickCommand();            // Close & ExecuteCommandLists
	dxCommon->GetFence()->WaitGPU();  // 完了待ち
	command->ResetCommand();           // 次回描画準備
}

Engine::Model* Engine::ModelManager::FindModel(const std::string& filePath)
{
    std::lock_guard<std::mutex> lock(modelsMutex_);
    const auto found = ModelAssetIdentity::Find(models, filePath);
    return found == models.end() ? nullptr : found->second.get();
}
bool Engine::ModelManager::IsLoading() const
{
	std::lock_guard<std::mutex> futureLock(futuresMutex_);
	return !loadingFutures_.empty();
}
