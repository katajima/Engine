#include "ModelManager.h"
#include"ModelCommon.h"
#include"Model.h"
#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/DirectX/DXGIDevice/DXGIDevice.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"

void ModelManager::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	command_ = dxCommon_->GetCommand();
	DXGIDevice_ = dxCommon_->GetDXGIDevice();
	srvManager_ = dxCommon_->GetSrvManager();
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
		// 実際の読み込み処理（LoadModel内部の処理を分解するイメージ）
		std::unique_ptr<Model> model = std::make_unique<Model>();
		model->Initialize(dxCommon_, modelCommon_.get(), "./resources/Models", filePath, dire);

		// 読み込み後にモデル登録（排他制御）
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
