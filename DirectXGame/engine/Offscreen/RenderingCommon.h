#pragma once
#include"DirectXGame/engine/base/TextureManager.h"

#include "DirectXGame/engine/struct/Structs3D.h"
#include "DirectXGame/engine/struct/VertexDeta.h"
#include "DirectXGame/engine/PSO/PSOManager.h"


class Camera;
class DirectXCommon;
class RenderingCommon
{
public:
	enum class PostEffectType {
		kCopy,			// コピー
		kGrayScale,		// グレースケール
		kSepia,			// セピア
		kVignette,      // ビネット
		kSmoothing,     // スムージング
		kGaussian,      // ガウス
		kOitline,		// アウトライン
	};

	// 初期化
	void Initialize(DirectXCommon* dxCommon);

	DirectXCommon* GetDxCommon() const { return dxCommon_; }

	void DrawCopyRender(int index);

	void DrawGrayScaleRender(int index);

	void DrawSepiaeRender(int index);

	void DrawVignetteRender(int index);

	void DrawSmoothingRender(int index);

	void DrawGaussianRender(int index);

	void DrawOutlineRender(int index);


	void SetCamera(Camera* camera) { camera_ = camera; }
	void UpdateImgui(PostEffectType type);

private:
	// ルートシグネチャの作成
	void CreateRootSignature();
	// グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();


private:
	void DrawColl();

	// コピー
	void DrawCopyImageSetting();

	// グレースケール
	void DrawGrayScaleSetting();

	// セピアケール
	void DrawSepiaSetting();

	// ビネット
	void DrawVignetteSetting();

	// スムージング
	void DrawSmoothingSetting();

	// ガウス
	void DrawGaussianSetting();

	// アウトライン
	void DrawOutlineSetting();


private:
	std::unique_ptr<PSOManager> psoManager_;
	DirectXCommon* dxCommon_;
	Camera* camera_;

	PostEffectType type_ = PostEffectType::kCopy;

	struct Rendering
	{
		//ルートシグネチャ
		Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature;
		// グラフィックスパイプラインステート
		Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState = nullptr;
	};
	Rendering copy_;
	Rendering grayScale_;
	Rendering sepia_;
	Rendering vignette_;
	Rendering smoothing_;
	Rendering gaussian_;
	Rendering outline_;

	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	Microsoft::WRL::ComPtr < ID3D12Resource> vertexResource;
	

private:
	struct VignetteGPU {
		float scale;
		float squared;
		float padding[2];
	};
	VignetteGPU* vignetteData_{};
	Microsoft::WRL::ComPtr < ID3D12Resource> vignetteResource_;
	
	struct SmoothigGPU {
		int num;
		float padding[3];
	};
	SmoothigGPU* smoothingData_{};
	Microsoft::WRL::ComPtr < ID3D12Resource> smoothingResource_;


	struct GaussianGPU {
		int num;
		float sigma;
		float padding[2];
	};
	GaussianGPU* gaussianData_{};
	Microsoft::WRL::ComPtr < ID3D12Resource> gaussianResource_;

	struct OutlineGPU {
		Matrix4x4 projectionInverse;
		int num;
		float weightSquared;
		float nearZ;
		float farZ;
	};
	OutlineGPU* outlineData_{};
	Microsoft::WRL::ComPtr < ID3D12Resource> outlineResource_;



};


