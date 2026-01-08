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
#include<map>
#include <memory>

#include <future>       // 追加：非同期処理用
#include <mutex>        // 追加：排他制御用
#include <shared_mutex> // C++17以降の共有ロック用（optional）

#include "DirectXGame/engine/struct/Structs3D.h"

namespace Engine {
	// 前方宣言
	class Model;
	class Command;
	class DXGIDevice;
	class SrvManager;
	class ModelCommon;
	class DirectXCommon;

	/// <summary>
	/// モデルを管理するクラス
	/// </summary>
	class ModelManager {
	public:
		ModelManager() = default;
		~ModelManager() = default;
		ModelManager(ModelManager&) = delete;
		ModelManager& operator=(ModelManager&) = delete;

		// 初期化
		void Initialize(DirectXCommon* dxCommon);

		/// <summary>
		/// モデルファイルの読み込み
		/// </summary>
		/// <param name="filePath"></param>
		void LoadModel(const std::string& filePath, const std::string& dire = "");

		// 非同期読み込み開始（マルチスレッド）
		void LoadModelAsync(const std::string& filePath, const std::string& dire = "");

		// 読み込み完了待ち（全て）
		void WaitAllLoadFinished();

		// モデル検索
		Model* FindModel(const std::string& filePath);

		// モデル全取得
		const std::map<std::string, std::unique_ptr<Model>>& GetModel() const { return models; }

		// モデル共通クラス取得
		ModelCommon* GetModelCommon() { return modelCommon_.get(); }

	private:
		std::map<std::string, std::unique_ptr<Model>> models;

		// 非同期読み込みで管理するfutureのリスト
		std::vector<std::future<void>> loadingFutures_;

		// 排他制御
		std::mutex modelsMutex_;  // modelsへの書き込みを守るmutex

		Command* command_;
		DXGIDevice* DXGIDevice_;
		SrvManager* srvManager_;
		DirectXCommon* dxCommon_;

		std::unique_ptr<ModelCommon> modelCommon_;

	};
}