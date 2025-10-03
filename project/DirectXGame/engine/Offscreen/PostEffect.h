#pragma once

#include "DirectXGame/engine/struct/VertexDeta.h"
#include "DirectXGame/engine/DirectX/Resource/ConstantBuffer.h"


#include "PostEffectData.h"
class Camera;
class DirectXCommon;
class PSOManager;
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

	void SetCamera(Camera* camera) { camera_ = camera; }






	ConstantBuffer<VignetteGPU>*	GetVignette(){return cbVignette_.get();}
	ConstantBuffer<SmoothigGPU>*	GetSmoothig() { return cbSmoothig_.get(); }
	ConstantBuffer<GaussianGPU>*	GetGaussian() { return cbGaussian_.get(); }
	ConstantBuffer<OutlineGPU>*		GetOutline() { return cbOutline_.get(); }
	ConstantBuffer<RadialBlurGPU>*	GetRadialBlur() { return cbRadialBlur_.get(); }
	ConstantBuffer<DissovleGPU>*	GetDissovle() { return cbDissovle_.get(); }
	ConstantBuffer<RandomGPU>*		GetRandom() { return cbRandom_.get(); }
	ConstantBuffer<BloomGPU>*		GetBloom() { return cbBloom_.get(); }

private:
	PostEffectType type_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	Microsoft::WRL::ComPtr < ID3D12Resource> vertexResource;
	DirectXCommon* dxCommon_ = nullptr;
	Camera* camera_ = nullptr;

	std::unique_ptr	<ConstantBuffer<VignetteGPU>>	cbVignette_;
	std::unique_ptr <ConstantBuffer<SmoothigGPU>>	cbSmoothig_;
	std::unique_ptr <ConstantBuffer<GaussianGPU>>	cbGaussian_;
	std::unique_ptr <ConstantBuffer<OutlineGPU>>	cbOutline_;
	std::unique_ptr <ConstantBuffer<RadialBlurGPU>> cbRadialBlur_;
	std::unique_ptr <ConstantBuffer<DissovleGPU>>	cbDissovle_;
	std::unique_ptr <ConstantBuffer<RandomGPU>>		cbRandom_;
	std::unique_ptr <ConstantBuffer<BloomGPU>>		cbBloom_;
};







class IPostEffect
{
public:
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

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

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
	void DrawRender(int index, int indexB) override;
protected:
	void CreateRootSignature() override;
};
// グレースケール
class PostEffectGrayScale : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
protected:
	void CreateRootSignature() override;
};
// セピア
class PostEffectSepia : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
protected:
	void CreateRootSignature() override;
};
// ビネット
class PostEffectVignette : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
protected:
	void CreateRootSignature() override;
};
// スムージング
class PostEffectSmoothing : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
protected:
	void CreateRootSignature() override;
};
// ガウシアン
class PostEffectGaussian : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
protected:
	void CreateRootSignature() override;
};
// アウトライン
class PostEffectOutline : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
protected:
	void CreateRootSignature() override;
};
// ラジアルブラー
class PostEffectRadialBlur : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
protected:
	void CreateRootSignature() override;
};
// ディゾルブ
class PostEffectDissovle : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
protected:
	void CreateRootSignature() override;
	// ノイズテクスチャ
	uint32_t dissovleIndex = 0;
};

// ランダム
class PostEffectRandom : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
protected:
	void CreateRootSignature() override;
};
// ブルーム
class PostEffectBloom : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
protected:
	void CreateRootSignature() override;
};

// 合成
class PostEffectCombin : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
protected:
	void CreateRootSignature() override;
};
