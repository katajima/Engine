#include "ModelManager.h"
#include"ModelCommon.h"
#include"Model.h"
#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void ModelManager::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;	// DX共通クラス

	command_ = dxCommon_->GetCommand();			// コマンド
	DXGIDevice_ = dxCommon_->GetDXGIDevice();	// デバイス
	srvManager_ = dxCommon_->GetSrvManager();	// SRV管理クラス

	// モデル共通クラス初期化
	modelCommon_ = std::make_unique<ModelCommon>();
	modelCommon_->Initialize(command_, DXGIDevice_, srvManager_);
}

void ModelManager::LoadModel(const std::string& filePath, const std::string& dire)
{

	std::string file = filePath;
	if (dire != "") {
		file = dire + filePath;
	}


	// 読み込み済みモデルを検索
	if (models.contains(file)) {
		return;
	}
	//モデルの生成とファイル読み込み、初期化
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(dxCommon_,modelCommon_.get(), "./resources/Models", filePath, dire);

	// モデルをmapコンテナに格納
	models.insert(std::make_pair(filePath, std::move(model)));
}

void ModelManager::LoadModelAsync(const std::string& filePath, const std::string& dire)
{
	// すでにロード済みかロックを使って確認
	{
		std::lock_guard<std::mutex> lock(modelsMutex_);
		if (models.contains(filePath)) {
			return;
		}
	}

	// 非同期に読み込み開始
	loadingFutures_.push_back(std::async(std::launch::async, [this, filePath, dire]() {
		std::unique_ptr<Model> model = std::make_unique<Model>();
		model->Initialize(dxCommon_, modelCommon_.get(), "./resources/Models", filePath, dire);

		// GPUにリソース記録が終わったあと、必ずキックして完了を待つ（非同期内）
		auto* command = dxCommon_->GetCommand();
		command->KickCommand();
		dxCommon_->GetFence()->WaitGPU();
		command->ResetCommand();

		{
			std::lock_guard<std::mutex> lock(modelsMutex_);
			models.insert(std::make_pair(filePath, std::move(model)));
		}
		}));
}

void ModelManager::WaitAllLoadFinished()
{
	for (auto& fut : loadingFutures_) {
		fut.get();  // 完了待ち
	}
	loadingFutures_.clear();

	
	auto* command = dxCommon_->GetCommand();
	command->KickCommand();            // Close & ExecuteCommandLists
	dxCommon_->GetFence()->WaitGPU();  // 完了待ち
	command->ResetCommand();           // 次回描画準備
}

Model* ModelManager::FindModel(const std::string& filePath)
{
	// 読み込み済みモデルを検索
	if (models.contains(filePath)) {
		// 読み込みモデルを戻り値としてreturn
		return models.at(filePath).get();
	}

	// ファイル名一致なし
	return nullptr;
}
