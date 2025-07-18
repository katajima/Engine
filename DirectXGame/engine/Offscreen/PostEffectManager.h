#pragma once

// C++
#include<vector>
#include<memory>

// engine
#include "DirectXGame/engine/DirectX/RenderTexture/RenderTexture.h"
#include "PostEffectBlock.h"






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
	void AddEffectBlock(const std::string name, PostEffectType type);

	//
	RenderTexture* GetEndRenderTexture() {
		if (effectBlocks_.empty()) return nullptr;
		return effectBlocks_.back()->GetEndRenderTexture();
	};

private:
	std::unique_ptr <RenderTexture> renderTexture_;
	std::vector<std::unique_ptr<PostEffectBlock>> effectBlocks_;
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

