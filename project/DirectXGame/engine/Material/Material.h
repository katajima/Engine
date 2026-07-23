#pragma once
#include "MaterialInstance.h"
#include "DirectXGame/engine/DirectX/Resource/ConstantBuffer.h"

#include "vector"
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
using namespace Microsoft::WRL;


namespace Engine {
	// 前方宣言
	class DirectXCommon;

	/// <summary>
	/// マテリアル
	/// </summary>
	class Material
	{
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* dxcommon);

		/// <summary>
		/// マテリアルコマンドリスト設定
		/// </summary>
		void GetCommandListMaterial(int index);

		/// <summary>
		/// テクスチャコマンドリスト設定
		/// </summary>
		void GetCommandListTexture(int indexDiffuse, int normalIndex = 0, int specularIndex = 0, int environmentIndex = 0);


		/// <summary>
		/// テクスチャ読み込み
		/// </summary>
		void LoadTex();


		/// <summary>
		/// マテリアルインスタンス取得
		/// </summary>
		MaterialInstance& GetMaterialInstance() { return materialInstance_; }


		/// <summary>
		/// GPUデータ更新
		/// </summary>
		void GPUData();
	public:

		// テクスチャ構造体
		struct Tex {
			std::string diffuseFilePath;
			std::string normalFilePath;
			std::string specularFilePath;
			std::string environmentFilePath;


			//テクスチャ番号
			uint32_t diffuseIndex = 0;
			uint32_t normalIndex = 0;
			uint32_t specularIndex = 0;
			uint32_t environmentIndex = 0;
		};
		Tex tex_;

		// GPUに送るデータ
		struct DataGPU
		{
			Color color;
			int32_t enableLighting;
			float alphaClipping = 0.5f;
			float environmentCoefficient = 0.5f;
			float alpha = 1.0f;
			Matrix4x4 uvTransform;
			float shininess;
			int32_t useLig;
			int32_t useHim;
			int32_t useNormalMap;
			int32_t useSpecularMap;
			float padding2[3];
		};


		void SetGPUMaterialInstance(const MaterialInstance& materialInstance, ConstantBuffer < Material::DataGPU>* cbResourcePtr);


	private:

		MaterialInstance materialInstance_;

	private:

		DirectXCommon* dxCommon = nullptr;
		std::unique_ptr<ConstantBuffer<Material::DataGPU>> cbResource_;
		// テクスチャ数
		uint32_t texDiffuseNum = 0;
	};
}
