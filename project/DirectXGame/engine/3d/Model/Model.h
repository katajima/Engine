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
		void DrawSkinning(std::vector<MaterialInstance> matetials, std::vector<ConstantBuffer<Material::DataGPU>*> cbResourcePtr);

		// ModelData取得
		ModelData& GetModelData() { return modelData; }
		// モデルのが透過物しているかを計算して透明度を返す
		float GetMaterialAlpha();

	public:
		
	private:
		// モデル共通クラス
		ModelCommon* modelCommon = nullptr;
		// SRV管理クラス
		SrvManager* srvManager = nullptr;
		// DirectX共通クラス
		DirectXCommon* dxCommon = nullptr;
	private:
		// モデルのデータ
		ModelData modelData;
		// 時間計測用
		DebugTimer timer_;
	public:
		// 読み込んだモデルからモデルデータ生成
		// ディレクトリパス
		// ファイル名
		ModelData LoadOdjFileAssimpAmime(const std::string& directoryPath, const std::string& filename);


	};
}