#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"
#include "DirectXGame/engine/Color/Color.h"
#include "DirectXGame/engine/DirectX/Resource/ConstantBuffer.h"

#include "vector"
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
using namespace Microsoft::WRL;
#include<d3d12.h>
#include<dxgi1_6.h>

namespace Engine {
	// 前方宣言
	class DirectXCommon;

	/// <summary>
	/// マテリアル
	/// </summary>
	class Material
	{
	public:
		// 初期化
		void Initialize(DirectXCommon* dxcommon);

		// マテリアルコマンドリスト設定
		void GetCommandListMaterial(int index);

		// テクスチャコマンドリスト設定
		void GetCommandListTexture(int indexDiffuse, int normalIndex = 0, int speculerIndex = 0, int environmentIndex = 0);

		// GPUデータ更新
		void GPUData();

		// テクスチャ読み込み
		void LoadTex();
	public:
		Transform transform;
		Color color;

		int32_t enableLighting_ = false;
		float  environmentCoefficient_ = 0.5f;
		float shininess_ = 64.0f;
		int32_t useLig_ = false;

		int32_t useNormalMap_ = 0;
		int32_t useSpeculerMap_ = 0;
		bool useEnvironment_ = 0;
		float alphaClipping_ = 0.5f;
		float alpha_ = 1.0f;

		// テクスチャ構造体
		struct Tex {
			std::string diffuseFilePath;
			std::string normalFilePath;
			std::string speculerFilePath;
			std::string environmentFilePath;


			//テクスチャ番号
			uint32_t diffuseIndex = 0;
			uint32_t normalIndex = 0;
			uint32_t speculerIndex = 0;
			uint32_t environmentIndex = 0;
		};
		Tex tex_;

	private:
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
			int32_t useSpeculerMap;
			float padding2[3];
		};
		DirectXCommon* dxCommon_ = nullptr;


		ConstantBuffer<Material::DataGPU> cbResource_;

		// テクスチャ数
		uint32_t texDiffuseNum = 0;
	};
}
