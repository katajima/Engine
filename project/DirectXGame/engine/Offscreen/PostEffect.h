#pragma once

#include "DirectXGame/engine/struct/VertexDeta.h"
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
		// 初期化
		void Initialize(DirectXCommon* dxCommon, PostEffectType type);

		// 描画
		void DrawRender();

		// ImGui
		void UpdateImgui();

		// 描画
		void DrawColl();
		// カメラ設定
		void SetCamera(Camera* camera) { this->camera = camera; }





		// ビネット取得
		ConstantBuffer<VignetteGPU>* GetVignette() { return cbVignette_.get(); }
		// スムージング取得
		ConstantBuffer<SmoothigGPU>* GetSmoothig() { return cbSmoothig_.get(); }
		// ガウス取得
		ConstantBuffer<GaussianGPU>* GetGaussian() { return cbGaussian_.get(); }
		// アウトライン取得
		ConstantBuffer<OutlineGPU>* GetOutline() { return cbOutline_.get(); }
		// ラジアルブラー取得
		ConstantBuffer<RadialBlurGPU>* GetRadialBlur() { return cbRadialBlur_.get(); }
		// ディゾルブ取得
		ConstantBuffer<DissovleGPU>* GetDissovle() { return cbDissovle_.get(); }
		// ランダム取得
		ConstantBuffer<RandomGPU>* GetRandom() { return cbRandom_.get(); }
		// ブルーム取得
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
		std::unique_ptr <ConstantBuffer<DissovleGPU>>	cbDissovle_;
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
		// ルートシグネチャとグラフィックパイプラインをまとめた構造体
		struct Rendering
		{
			//ルートシグネチャ
			Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
			// グラフィックスパイプラインステート
			Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState = nullptr;
		};

		// 初期化
		void Initialize(DirectXCommon* dxCommon, std::string psName);

		// 描画
		virtual void DrawRender(int index, int indexB = 0) = 0;
	protected:
		// 共通パイプライン生成
		void CreateCommonPipeline(std::string psName);
		// 描画設定
		void DrawSetting();
	public: // 取得or設定
		//  DirectX共通クラス取得
		DirectXCommon* GetDxCommon() const { return dxCommon_; }
		// グラフィックパイプラインとルートシグネチャ取得
		Rendering& GetRendering() { return posteffect_; };
	protected:
		// ルートシグネチャの作成
		virtual void CreateRootSignature() = 0;
	protected:
		Rendering posteffect_;
		std::unique_ptr<PSOManager> psoManager_;
		DirectXCommon* dxCommon_ = nullptr;
	};


	// コピー
	class PostEffectCopy : public IPostEffect {
	public:
		// 描画
		void DrawRender(int index, int indexB) override;
	protected:
		// ルートシグネチャ生成
		void CreateRootSignature() override;
	};
	// グレースケール
	class PostEffectGrayScale : public IPostEffect {
	public:
		// 描画
		void DrawRender(int index, int indexB) override;
	protected:
		// ルートシグネチャ生成
		void CreateRootSignature() override;
	};
	// セピア
	class PostEffectSepia : public IPostEffect {
	public:
		// 描画
		void DrawRender(int index, int indexB) override;
	protected:
		// ルートシグネチャ生成
		void CreateRootSignature() override;
	};
	// ビネット
	class PostEffectVignette : public IPostEffect {
	public:
		// 描画
		void DrawRender(int index, int indexB) override;
	protected:
		// ルートシグネチャ生成
		void CreateRootSignature() override;
	};
	// スムージング
	class PostEffectSmoothing : public IPostEffect {
	public:
		// 描画
		void DrawRender(int index, int indexB) override;
	protected:
		// ルートシグネチャ生成
		void CreateRootSignature() override;
	};
	// ガウシアン
	class PostEffectGaussian : public IPostEffect {
	public:
		// 描画
		void DrawRender(int index, int indexB) override;
	protected:
		// ルートシグネチャ生成
		void CreateRootSignature() override;
	};
	// アウトライン
	class PostEffectOutline : public IPostEffect {
	public:
		// 描画
		void DrawRender(int index, int indexB) override;
	protected:
		// ルートシグネチャ生成
		void CreateRootSignature() override;
	};
	// ラジアルブラー
	class PostEffectRadialBlur : public IPostEffect {
	public:
		// 描画
		void DrawRender(int index, int indexB) override;
	protected:
		// ルートシグネチャ生成
		void CreateRootSignature() override;
	};
	// ディゾルブ
	class PostEffectDissovle : public IPostEffect {
	public:
		// 描画
		void DrawRender(int index, int indexB) override;
	protected:
		// ルートシグネチャ生成
		void CreateRootSignature() override;
		// ノイズテクスチャ
		uint32_t dissovleIndex = 0;
	};

	// ランダム
	class PostEffectRandom : public IPostEffect {
	public:
		// 描画
		void DrawRender(int index, int indexB) override;
	protected:
		// ルートシグネチャ生成
		void CreateRootSignature() override;
	};
	// ブルーム
	class PostEffectBloom : public IPostEffect {
	public:
		// 描画
		void DrawRender(int index, int indexB) override;
	protected:
		// ルートシグネチャ生成
		void CreateRootSignature() override;
	};

	// 合成
	class PostEffectCombin : public IPostEffect {
	public:
		// 描画
		void DrawRender(int index, int indexB) override;
	protected:
		// ルートシグネチャ生成
		void CreateRootSignature() override;
	};
}