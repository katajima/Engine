#pragma once

#include "DirectXGame/engine/struct/VertexData.h"
#include "DirectXGame/engine/DirectX/Resource/ConstantBuffer.h"


#include "PostEffectData.h"


namespace Engine {
	// 前方宣言
	class Camera;
	class DirectXCommon;
	class PSOManager;

	/// <summary>
	/// ポストエフェクトデータクラス
	/// </summary>
	class PostEffectData
	{
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* dxCommon, PostEffectType type);

		/// <summary>
		/// 描画
		/// </summary>
		void DrawRender();

		/// <summary>
		/// ImGui
		/// </summary>
		void UpdateImgui();

		/// <summary>
		/// 描画
		/// </summary>
		void DrawColl();
		/// <summary>
		/// カメラ設定
		/// </summary>
		void SetCamera(Camera* camera) { this->camera = camera; }





		/// <summary>
		/// ビネット取得
		/// </summary>
		ConstantBuffer<VignetteGPU>* GetVignette() { return cbVignette_.get(); }
		/// <summary>
		/// スムージング取得
		/// </summary>
		ConstantBuffer<SmoothigGPU>* GetSmoothig() { return cbSmoothig_.get(); }
		/// <summary>
		/// ガウス取得
		/// </summary>
		ConstantBuffer<GaussianGPU>* GetGaussian() { return cbGaussian_.get(); }
		/// <summary>
		/// アウトライン取得
		/// </summary>
		ConstantBuffer<OutlineGPU>* GetOutline() { return cbOutline_.get(); }
		/// <summary>
		/// ラジアルブラー取得
		/// </summary>
		ConstantBuffer<RadialBlurGPU>* GetRadialBlur() { return cbRadialBlur_.get(); }
		/// <summary>
		/// ディゾルブ取得
		/// </summary>
		ConstantBuffer<DissolveGPU>* GetDissolve() { return cbDissolve_.get(); }
		/// <summary>
		/// ランダム取得
		/// </summary>
		ConstantBuffer<RandomGPU>* GetRandom() { return cbRandom_.get(); }
		/// <summary>
		/// ブルーム取得
		/// </summary>
		ConstantBuffer<BloomGPU>* GetBloom() { return cbBloom_.get(); }

	private:
		PostEffectType type_;

		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		Microsoft::WRL::ComPtr < ID3D12Resource> vertexResource;

		std::unique_ptr	<ConstantBuffer<VignetteGPU>>	cbVignette_;
		std::unique_ptr <ConstantBuffer<SmoothigGPU>>	cbSmoothig_;
		std::unique_ptr <ConstantBuffer<GaussianGPU>>	cbGaussian_;
		std::unique_ptr <ConstantBuffer<OutlineGPU>>	cbOutline_;
		std::unique_ptr <ConstantBuffer<RadialBlurGPU>> cbRadialBlur_;
		std::unique_ptr <ConstantBuffer<DissolveGPU>>	cbDissolve_;
		std::unique_ptr <ConstantBuffer<RandomGPU>>		cbRandom_;
		std::unique_ptr <ConstantBuffer<BloomGPU>>		cbBloom_;
	private:
		DirectXCommon* dxCommon = nullptr;
		Camera* camera = nullptr;
	};






	/// <summary>
	/// ポストエフェクト基底クラス
	/// </summary>
	class IPostEffect
	{
	public:
		/// <summary>
		/// 派生ポストエフェクトをIPostEffect経由で破棄しても、派生側まで正しく解放する。
		/// </summary>
		virtual ~IPostEffect() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* dxCommon, std::string psName);

		/// <summary>
		/// 描画
		/// </summary>
		virtual void DrawRender(int index, int indexB = 0) = 0;
	protected:
		/// <summary>
		/// 共通パイプライン生成
		/// </summary>
		void CreateCommonPipeline(std::string psName);
		/// <summary>
		/// 描画設定
		/// </summary>
		void DrawSetting();
	public: // 取得or設定
		/// <summary>
		/// DirectX共通クラス取得
		/// </summary>
		DirectXCommon* GetDxCommon() const { return dxCommon_; }
		/// <summary>
		/// グラフィックパイプラインとルートシグネチャ取得
		/// </summary>
		PSRS& GetRendering() { return posteffect_; };
	protected:
		/// <summary>
		/// ルートシグネチャの作成
		/// </summary>
		virtual void CreateRootSignature() = 0;
	protected:
		PSRS posteffect_; // ポストエフェクト用ルートシグネチャとPSO
		std::unique_ptr<PSOManager> psoManager_;
		DirectXCommon* dxCommon_ = nullptr;
	};


	// コピー
/// <summary>
/// PostEffectCopyを管理・実装するクラス。
/// </summary>
	class PostEffectCopy : public IPostEffect {
	public:
		/// <summary>
		/// 描画
		/// </summary>
		void DrawRender(int index, int indexB) override;
	protected:
		/// <summary>
		/// ルートシグネチャ生成
		/// </summary>
		void CreateRootSignature() override;
	};
	// グレースケール
/// <summary>
/// PostEffectGrayScaleを管理・実装するクラス。
/// </summary>
	class PostEffectGrayScale : public IPostEffect {
	public:
		/// <summary>
		/// 描画
		/// </summary>
		void DrawRender(int index, int indexB) override;
	protected:
		/// <summary>
		/// ルートシグネチャ生成
		/// </summary>
		void CreateRootSignature() override;
	};
	// セピア
/// <summary>
/// PostEffectSepiaを管理・実装するクラス。
/// </summary>
	class PostEffectSepia : public IPostEffect {
	public:
		/// <summary>
		/// 描画
		/// </summary>
		void DrawRender(int index, int indexB) override;
	protected:
		/// <summary>
		/// ルートシグネチャ生成
		/// </summary>
		void CreateRootSignature() override;
	};
	// ビネット
/// <summary>
/// PostEffectVignetteを管理・実装するクラス。
/// </summary>
	class PostEffectVignette : public IPostEffect {
	public:
		/// <summary>
		/// 描画
		/// </summary>
		void DrawRender(int index, int indexB) override;
	protected:
		/// <summary>
		/// ルートシグネチャ生成
		/// </summary>
		void CreateRootSignature() override;
	};
	// スムージング
/// <summary>
/// PostEffectSmoothingを管理・実装するクラス。
/// </summary>
	class PostEffectSmoothing : public IPostEffect {
	public:
		/// <summary>
		/// 描画
		/// </summary>
		void DrawRender(int index, int indexB) override;
	protected:
		/// <summary>
		/// ルートシグネチャ生成
		/// </summary>
		void CreateRootSignature() override;
	};
	// ガウシアン
/// <summary>
/// PostEffectGaussianを管理・実装するクラス。
/// </summary>
	class PostEffectGaussian : public IPostEffect {
	public:
		/// <summary>
		/// 描画
		/// </summary>
		void DrawRender(int index, int indexB) override;
	protected:
		/// <summary>
		/// ルートシグネチャ生成
		/// </summary>
		void CreateRootSignature() override;
	};
	// アウトライン
/// <summary>
/// PostEffectOutlineを管理・実装するクラス。
/// </summary>
	class PostEffectOutline : public IPostEffect {
	public:
		/// <summary>
		/// 描画
		/// </summary>
		void DrawRender(int index, int indexB) override;
	protected:
		/// <summary>
		/// ルートシグネチャ生成
		/// </summary>
		void CreateRootSignature() override;
	};
	// ラジアルブラー
/// <summary>
/// PostEffectRadialBlurを管理・実装するクラス。
/// </summary>
	class PostEffectRadialBlur : public IPostEffect {
	public:
		/// <summary>
		/// 描画
		/// </summary>
		void DrawRender(int index, int indexB) override;
	protected:
		/// <summary>
		/// ルートシグネチャ生成
		/// </summary>
		void CreateRootSignature() override;
	};
	// ディゾルブ
/// <summary>
/// PostEffectDissolveを管理・実装するクラス。
/// </summary>
	class PostEffectDissolve : public IPostEffect {
	public:
		/// <summary>
		/// 描画
		/// </summary>
		void DrawRender(int index, int indexB) override;
	protected:
		/// <summary>
		/// ルートシグネチャ生成
		/// </summary>
		void CreateRootSignature() override;
		// ノイズテクスチャ
		uint32_t dissolveIndex = 0;
	};

	// ランダム
/// <summary>
/// PostEffectRandomを管理・実装するクラス。
/// </summary>
	class PostEffectRandom : public IPostEffect {
	public:
		/// <summary>
		/// 描画
		/// </summary>
		void DrawRender(int index, int indexB) override;
	protected:
		/// <summary>
		/// ルートシグネチャ生成
		/// </summary>
		void CreateRootSignature() override;
	};
	// ブルーム
/// <summary>
/// PostEffectBloomを管理・実装するクラス。
/// </summary>
	class PostEffectBloom : public IPostEffect {
	public:
		/// <summary>
		/// 描画
		/// </summary>
		void DrawRender(int index, int indexB) override;
	protected:
		/// <summary>
		/// ルートシグネチャ生成
		/// </summary>
		void CreateRootSignature() override;
	};

	// 合成
/// <summary>
/// PostEffectCombineを管理・実装するクラス。
/// </summary>
	class PostEffectCombine : public IPostEffect {
	public:
		/// <summary>
		/// 描画
		/// </summary>
		void DrawRender(int index, int indexB) override;
	protected:
		/// <summary>
		/// ルートシグネチャ生成
		/// </summary>
		void CreateRootSignature() override;
	};
}
