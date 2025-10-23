#pragma once

// C++
#include<vector>
#include<memory>

// engine
#include "DirectXGame/engine/DirectX/RenderTexture/RenderTexture.h"
#include "PostEffectBlock.h"

#include "DirectXGame/engine/base/WinApp/WinApp.h"


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
	void Intialize(DXGIDevice* DXGIDevice, Command* command,
		SrvManager* srvManager, RtvManager* rvtManager,
		RenderingCommon* renderingCommon, DepthStencil* depthStencil,
		Barrier* barrier, ScissorRect* scissorRect,
		ViewPort* viewPort);

	// 最初
	void PreDrawOffscreen();

	// 最初
	void PostDrawOffscreen();

	// 2D
	void PreDraw2dOffscreen();

	// 2D
	void PostDraw2dOffscreen();

	// 全ポストエフェクト処理
	void AllPostEffect(SceneManager* sceneManager);

	// 更新
	void Update(Camera* camera);


	// ポストエフェクトブロック追加
	void AddEffectBlocks(std::vector<PostEffectBlock*> effectBlocks);

	// 最終レンダーテクスチャ取得
	RenderTexture* GetEndRenderTexture() { return renderTextureEnd_.get(); };

	// ポストエフェクトブロッククリア
	void ClearPostEffectBlock();
	// ImGui使用時の計算処理
	void RenderImGui();
	// サイズ更新
	void RenderUpdate();

	// ImGui画像左上座標取得
	Vector2 GetImageleftTopPos() const {
#ifdef _DEBUG
		return imageleftTopPos_;
#else
		return Vector2(0, 0);
#endif
	}

	// ImGui画像サイズ取得
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

	// ImGui画像画面比率取得
	Vector2 GetImageRatio() const { return imageRatio_; }

private:
	// 前レンダーテクスチャ
	void PreEnd(RenderTexture* renderTexture);
	// 後レンダーテクスチャ
	void PostEnd(RenderTexture* renderTexture);

private:
	std::unique_ptr<RenderTexture> renderTexture_;
	std::unique_ptr<RenderTexture> renderTextureEnd_;

	std::vector<PostEffectBlock*> effectBlocks_;

	uint32_t indexCount_ = 0;

	Vector2 imageleftTopPos_;
	Vector2 imageSize_;
	Vector2 imageRatio_;

private:
	DXGIDevice* DXGIDevice_;
	Command* command_;
	SrvManager* srvManager_;
	RtvManager* rtvManager_;
	RenderingCommon* renderingCommon_;
	DepthStencil* depthStencil_;
	Barrier* barrier_;
	ScissorRect* scissorRect_;
	ViewPort* viewPort_;
};
