#pragma once
#include "PostEffect.h"


namespace Engine {
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
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(DirectXCommon* dxCommon);
		/// <summary>
		/// 描画
		/// </summary>
		void DrawRender(PostEffectType type, int index, int indexB = 0);
		/// <summary>
		/// DirectX共通クラス取得
		/// </summary>
		DirectXCommon* GetDxCommon() const { return dxCommon; }
		/// <summary>
		/// カメラ設定
		/// </summary>
		void SetCamera(Camera* camera) { this->camera = camera; }

	private:
		DirectXCommon* dxCommon = nullptr;
		Camera* camera = nullptr;

		PostEffectType type_ = PostEffectType::kCopy;

		PostEffectCopy			copy_;
		PostEffectGrayScale		grayScale_;
		PostEffectSepia			sepia_;
		PostEffectVignette		vignette_;
		PostEffectSmoothing		smoothing_;
		PostEffectGaussian		gaussian_;
		PostEffectOutline		outline_;
		PostEffectRadialBlur	radialBlur_;
		PostEffectDissolve		dissolve_;
		PostEffectRandom		random_;
		PostEffectBloom			bloom_;
		PostEffectCombine		combine_;
	};
}


