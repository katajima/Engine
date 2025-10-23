#pragma once
#include "PostEffect.h"


// 前方宣言
class Camera;
class DirectXCommon;

/// <summary>
/// レンダリング共通クラス
/// </summary>
class RenderingCommon
{
private:

public:
	// 初期化
	void Initialize(DirectXCommon* dxCommon);
	// 描画
	void DrawRender(PostEffectType type, int index, int indexB = 0);
	// DirectX共通クラス取得
	DirectXCommon* GetDxCommon() const { return dxCommon_; }
	// カメラ設定
	void SetCamera(Camera* camera) { camera_ = camera; }

private:
	DirectXCommon* dxCommon_;
	Camera* camera_;

	PostEffectType type_ = PostEffectType::kCopy;

	PostEffectCopy			copy_;
	PostEffectGrayScale		grayScale_;
	PostEffectSepia			sepia_;
	PostEffectVignette		vignette_;
	PostEffectSmoothing		smoothing_;
	PostEffectGaussian		gaussian_;
	PostEffectOutline		outline_;
	PostEffectRadialBlur	radialBlur_;
	PostEffectDissovle		dissovle_;
	PostEffectRandom		random_;
	PostEffectBloom			bloom_;
	PostEffectCombin		cimbin_;
};


