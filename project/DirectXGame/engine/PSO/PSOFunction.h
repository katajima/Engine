#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
#include<cstdint>
#include<wrl.h>
using namespace Microsoft::WRL;
#include<list>
#include<string>
#include<vector>
#include<format>

#include "DirectXGame/engine/base/Logger.h"
#include "DirectXGame/engine/Utility/StringUtility.h"


namespace Engine {
	// テクスチャ方法
	enum class TextureAddressMode {
		kWRAP, // テクスチャの境界を超える座標の場合、全体の画像を繰り返して表示します。つまり、テクスチャ座標の小数部分を使用して、画像が連続しているかのように見せる方法です。
		kMIRROR,// テクスチャが折り返され、隣接する部分が左右（または上下）に鏡映（ミラーリング）されます。これにより、境界で反転した画像が連続して表示される効果が得られます。
		kCLAMP,// テクスチャ座標が範囲外の場合、最も近い境界のピクセルを使用します。結果として、エッジの色で延長されるような効果になります。
		kBORDER,// 範囲外の座標に対して、事前に設定されたボーダーカラー（境界色）を返す方式です。これにより、画像の境界外は一定の色で塗りつぶされるような効果が得られます。
		kMIRROR_ONCE,// 一度ミラーリングを適用し、その後は範囲外部分をクランプする方式です。最初の一回は反転させ、次からはエッジの値を使用するため、特殊な効果が実現できます。
	};

	// シェーダーファイル種類
	enum class ShaderFileName
	{
		PS,
		VS,
		DS,
		HS,
		GS,
		AS,
		MS,
	};

	// 前方宣言
	class DXGIDevice;

	// PSO設定関数
	namespace PSOFunction {
		/// <summary>
		/// 標準的なアルファブレンド設定を生成する
		/// </summary>
		D3D12_BLEND_DESC CreateAlphaBlendDesc();
		/// <summary>
		/// 深度テストを有効にした標準的な深度ステンシル設定を生成する
		/// </summary>
		D3D12_DEPTH_STENCIL_DESC CreateDepthStencilDesc();
		/// <summary>
		/// ルートパラメータ設定
		/// </summary>
		void SetRootParameter(D3D12_ROOT_PARAMETER& parameter, int ShaderRegister, D3D12_SHADER_VISIBILITY shaderType, D3D12_ROOT_PARAMETER_TYPE rootType);
		/// <summary>
		/// ルートパラメータ設定
		/// </summary>
		void SetRootParameter(D3D12_ROOT_PARAMETER& parameter, D3D12_DESCRIPTOR_RANGE& descriptorRange, D3D12_SHADER_VISIBILITY shaderType);
		/// <summary>
		/// ディスクリプタレンジ設定
		/// </summary>
		void SetDescriptorRange(D3D12_DESCRIPTOR_RANGE& descriptorRange, int ShaderRegister, int numDescriptors, D3D12_DESCRIPTOR_RANGE_TYPE rangeType);
		/// <summary>
		/// サンプラー設定
		/// </summary>
		void SetSampler(D3D12_STATIC_SAMPLER_DESC& staticSamplers, int shaderRegister, D3D12_FILTER filter, D3D12_SHADER_VISIBILITY shaderType, TextureAddressMode mode = TextureAddressMode::kWRAP);
		/// <summary>
		/// バルブ設定
		/// </summary>
		void Blob(DXGIDevice* DXGIDevice, D3D12_ROOT_SIGNATURE_DESC descriptionSignature, Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature);
	};
}
