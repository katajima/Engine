#pragma once

// C++
#include<vector>
#include<memory>

// engine
#include "DirectXGame/engine/DirectX/RenderTexture/RenderTexture.h"
#include "PostEffectBlock.h"

#include "DirectXGame/engine/base/WinApp/WinApp.h"

namespace Engine {
	// 前方宣言
	class DXGIDevice;
	class Command;
	class SrvManager;
	class RtvManager;
	class RenderingCommon;
	class DepthStencil;
	class Barrier;
	class ScissorRect;
	class ViewPort;
	class SceneManager;

	/// <summary>
	/// ポストエフェクトマネージャークラス
	/// </summary>
	class PostEffectManager {
	public:
		// 初期化
		void Initialize(DXGIDevice* DXGIDevice, Command* command,
			SrvManager* srvManager, RtvManager* rvtManager,
			RenderingCommon* renderingCommon, DepthStencil* depthStencil,
			Barrier* barrier, ScissorRect* scissorRect,
			ViewPort* viewPort);

		/// <summary>
		/// 最初
		/// </summary>
		void PreDrawOffscreen();

		/// <summary>
		/// 最初
		/// </summary>
		void PostDrawOffscreen();

		/// <summary>
		/// 2D
		/// </summary>
		void PreDraw2dOffscreen();

		/// <summary>
		/// 2D
		/// </summary>
		void PostDraw2dOffscreen();

		/// <summary>
		/// 全ポストエフェクト処理
		/// </summary>
		void AllPostEffect(SceneManager* sceneManager);

		/// <summary>
		/// 更新
		/// </summary>
		void Update(Camera* camera);


		/// <summary>
		/// ポストエフェクトパイプライン追加
		/// </summary>
		void AddPipeline(PostEffectPipeline* pipeline);

		/// <summary>
		/// 最終レンダーテクスチャ取得
		/// </summary>
		RenderTexture* GetEndRenderTexture() { return renderTextureEnd_.get(); };

		/// <summary>
		/// ポストエフェクトパイプラインクリア
		/// </summary>
		void ClearPostEffectBlock();
		/// <summary>
		/// ImGui使用時の計算処理
		/// </summary>
		void RenderImGui();
		/// <summary>
		/// サイズ更新
		/// </summary>
		void RenderUpdate();

		/// <summary>
		/// ImGui画像左上座標取得
		/// </summary>
		Vector2 GetImageleftTopPos() const {
#ifdef _DEBUG
			return imageleftTopPos_;
#else
			return Vector2(0, 0);
#endif
		}

		/// <summary>
		/// ImGui画像サイズ取得
		/// </summary>
		Vector2 GetImageSize() const {
#ifdef _DEBUG
			return imageSize_;
#else
			RECT rect;
			GetClientRect(WinApp::GetHwnd(), &rect);
			const int width = rect.right - rect.left;
			const int height = rect.bottom - rect.top;
			return Vector2(static_cast<float>(width), static_cast<float>(height));
#endif
		}

		/// <summary>
		/// ImGui画像画面比率取得
		/// </summary>
		Vector2 GetImageRatio() const { return imageRatio_; }

	private:
		/// <summary>
		/// 前レンダーテクスチャ
		/// </summary>
		void PreEnd(RenderTexture* renderTexture);
		/// <summary>
		/// 後レンダーテクスチャ
		/// </summary>
		void PostEnd(RenderTexture* renderTexture);

	private:
		std::unique_ptr<RenderTexture> renderTexture_;
		std::unique_ptr<RenderTexture> renderTextureEnd_;

		std::vector<PostEffectPipeline*> pipelines_;

		uint32_t indexCount_ = 0;

		Vector2 imageleftTopPos_;
		Vector2 imageSize_;
		Vector2 imageRatio_;

	private:
		DXGIDevice* dxgiDevice = nullptr;
		Command* command = nullptr;
		SrvManager* srvManager = nullptr;
		RtvManager* rtvManager = nullptr;
		RenderingCommon* renderingCommon = nullptr;
		DepthStencil* depthStencil = nullptr;
		Barrier* barrier = nullptr;
		ScissorRect* scissorRect = nullptr;
		ViewPort* viewPort = nullptr;
	};
}
