#pragma once
#include"DirectXGame/engine/base/Texture/TextureManager.h"
#include "DirectXGame/engine/struct/VertexDeta.h"
#include "DirectXGame/engine/PSO/PSOManager.h"
#include "DirectXGame/engine/DirectX/Resource/ConstantBuffer.h"


#include "PostEffectData.h"

class Camera;
class DirectXCommon;
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

	// ImGui
	virtual void UpdateImgui() = 0;

protected:
	// 共通パイプライン生成
	void CreateCommonPipeline(std::string psName);
	// 描画設定
	void DrawSetting();
	// 描画
	void DrawColl();
public: // 取得or設定

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	void SetCamera(Camera* camera) { camera_ = camera; }

	Rendering& GetRendering() { return posteffect_; };

protected:
	// ルートシグネチャの作成
	virtual void CreateRootSignature() = 0;

	// リソース生成
	virtual void CreateBuffer() = 0;

protected:
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	Microsoft::WRL::ComPtr < ID3D12Resource> vertexResource;



	Rendering posteffect_;
	std::unique_ptr<PSOManager> psoManager_;
	DirectXCommon* dxCommon_ = nullptr;
	Camera* camera_ = nullptr;
};


// コピー
class PostEffectCopy : public IPostEffect{
public:
	void DrawRender(int index, int indexB) override;
	void UpdateImgui() override;
protected:
	void CreateRootSignature() override;
	void CreateBuffer() override;
};
// グレースケール
class PostEffectGrayScale : public IPostEffect{
public:
	void DrawRender(int index, int indexB) override;
	void UpdateImgui() override;
protected:
	void CreateRootSignature() override;
	void CreateBuffer() override;
};
// セピア
class PostEffectSepia : public IPostEffect{
public:
	void DrawRender(int index, int indexB) override;
	void UpdateImgui() override;
protected:
	void CreateRootSignature() override;
	void CreateBuffer() override;
};
// ビネット
class PostEffectVignette : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
	void UpdateImgui() override;
protected:
	void CreateRootSignature() override;
	void CreateBuffer() override;
	ConstantBuffer<VignetteGPU> cbVignette_;
};
// スムージング
class PostEffectSmoothing : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
	void UpdateImgui() override;
protected:
	void CreateRootSignature() override;
	void CreateBuffer() override;
	ConstantBuffer<SmoothigGPU> cbSmoothig_;
};
// ガウシアン
class PostEffectGaussian : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
	void UpdateImgui() override;
protected:
	void CreateRootSignature() override;
	void CreateBuffer() override;
	ConstantBuffer<GaussianGPU> cbGaussian_;
};
// アウトライン
class PostEffectOutline : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
	void UpdateImgui() override;
protected:
	void CreateRootSignature() override;
	void CreateBuffer() override;

	ConstantBuffer<OutlineGPU> cbOutline_;
};
// ラジアルブラー
class PostEffectRadialBlur : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
	void UpdateImgui() override;
protected:
	void CreateRootSignature() override;
	void CreateBuffer() override;
	ConstantBuffer<RadialBlurGPU> cbRadialBlur_;
};
// ディゾルブ
class PostEffectDissovle : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
	void UpdateImgui() override;
protected:
	void CreateRootSignature() override;
	void CreateBuffer() override;
	ConstantBuffer<DissovleGPU> cbDissovle_;
	// ノイズテクスチャ
	uint32_t dissovleIndex = 0;
};

// ランダム
class PostEffectRandom : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
	void UpdateImgui() override;
protected:
	void CreateRootSignature() override;
	void CreateBuffer() override;
	ConstantBuffer<RandomGPU> cbRandom_;
};
// ブルーム
class PostEffectBloom : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
	void UpdateImgui() override;
protected:
	void CreateRootSignature() override;
	void CreateBuffer() override;
	ConstantBuffer<BloomGPU> cbBloom_;
};

// 合成
class PostEffectCombin : public IPostEffect {
public:
	void DrawRender(int index, int indexB) override;
	void UpdateImgui() override;
protected:
	void CreateRootSignature() override;
	void CreateBuffer() override;
};
