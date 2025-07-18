#pragma once
// C++
#include<vector>
#include<memory>

// engine
#include "DirectXGame/engine/DirectX/RenderTexture/RenderTexture.h"

class DXGIDevice;
class Command;
class SrvManager;
class RtvManager;
class RenderingCommon;
class DepthStencil;
class Barrier;
class ScissorRect;
class ViewPort;

class PostEffectBlock 
{
public:

	/// <summary>
	///  初期化
	/// </summary>
	void Intialize(DXGIDevice* DXGIDevice, Command* command, SrvManager* srvManager, RtvManager* rvtManager, RenderingCommon* renderingCommon,
		DepthStencil* depthStencil, Barrier* barrier, ScissorRect* scissorRect, ViewPort* viewPort);

	void Update(Camera* camera);

	// レンダーテクスチャ追加
	void AddRenderTexture(const std::string name, RenderTexture::PostEffectType type);

	
	// テクスチャ書き込み
	void DrawRenderTexture(RenderTexture* renderTextureRenderTreget, RenderTexture* renderTexturePixelSheder);

	// エフェクト
	void DrawEffectBlock();

	// 最初
	RenderTexture* GetFirstRenderTexture() {
		return renderTextures_.begin()->get();
	};
	// 最後
	RenderTexture* GetEndRenderTexture() {
		return renderTextures_.end()->get();
	};
private:
	// レンダーテクスチャ描画前処理
	void PreDraw(RenderTexture* renderTexture);
	// レンダーテクスチャ描画後処理
	void PostDraw(RenderTexture* renderTexture);

private:

	// レンダーテクスチャたち
	std::vector<std::unique_ptr<RenderTexture>> renderTextures_;
	// 最後
	RenderTexture* endRenderTexture = nullptr;
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
