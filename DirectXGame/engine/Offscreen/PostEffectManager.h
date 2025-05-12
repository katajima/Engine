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

class PostEffectManager
{
public:
	void Intialize(DXGIDevice* DXGIDevice, Command* command, SrvManager* srvManager, RtvManager* rvtManager, RenderingCommon* renderingCommon, DepthStencil* depthStencil, Barrier* barrier, ScissorRect* scissorRect,ViewPort* viewPort);

	// 最初(シーンに書き込み)
	void PreDrawOffscreen();

	// 最初
	void PostDrawOffscreen();

	void AllPostEffect();


	void Update(Camera* camera);

	// レンダーテクスチャ追加
	void AddRenderTexture(std::string name);

	RenderTexture* GetFirstRenderTexture() {return  renderTextures_[0].get(); };
	//
	RenderTexture* GetEndRenderTexture() { return  renderTextures_[renderTextureIndex_ - 1].get(); };

private:

	void DrawRenderTexture(RenderTexture* renderTextureRenderTreget, RenderTexture* renderTexturePixelSheder);

	// レンダーテクスチャ描画前処理
	void PreDraw(RenderTexture* renderTexture);
	// レンダーテクスチャ描画後処理
	void PostDraw(RenderTexture* renderTexture);

	

private:

	// 
	std::vector<std::unique_ptr<RenderTexture>> renderTextures_;

	// レンダーテクスチャ番号
	uint32_t renderTextureIndex_ = 0;

	RenderTexture* firstRenderTexture;
	RenderTexture* endRenderTexture;
private:
	bool isFirst_ = false;


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

