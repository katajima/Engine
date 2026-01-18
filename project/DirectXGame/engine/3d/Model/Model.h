#pragma once
#include"ModelData.h"

// C++
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<string>
#include<vector>
#include<format>
#include<span>
#include <iostream>
#include <memory>


#include "DirectXGame/engine/Utility/TimerUtility.h"

namespace Engine {
	// 前方宣言
	class ModelCommon;
	class DirectXCommon;
	class Material;

	/// <summary>
	/// モデルクラス
	/// </summary>
	class Model
	{
	public:

		/// <summary>
		/// モデルの初期化
		/// </summary>
		/// <param name="dxCommon">DirectXの共通クラス</param>
		/// <param name="modelCommon">モデルの共通クラス</param>
		/// <param name="directorypath">ディレクトリパス</param>
		/// <param name="filename">ファイル名(モデルの名前(objやgltf))</param>
		/// <param name="file">ファイル名(resources/models以降にファイルがあるならモデルの入っているファイル名を)</param>
		void Initialize(DirectXCommon* dxCommon, ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename, const std::string& file = "");

		// 通常描画
		void Draw();

		// スキニング用描画
		void DrawSkinning(std::vector<MaterialInstance> matetials);

		// ModelData取得
		ModelData& GetModelData() { return modelData; }
		// モデルのが透過物しているかを計算して透明度を返す
		float GetMaterialAlpha();

	public:
		// モデルのデータ
		ModelData modelData;
	private:
		ModelCommon* modelCommon_ = nullptr;
		SrvManager* srvManager_ = nullptr;
		DirectXCommon* dxCommon_;
		DebugTimer timer_;
	public:
		// 読み込んだモデルからモデルデータ生成
		// ディレクトリパス
		// ファイル名
		ModelData LoadOdjFileAssimpAmime(const std::string& directoryPath, const std::string& filename);


	};
}