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

class SceneManager;

class PostEffectManager
{
public:
	void Intialize(DXGIDevice* DXGIDevice, Command* command, SrvManager* srvManager, RtvManager* rvtManager, RenderingCommon* renderingCommon, DepthStencil* depthStencil, Barrier* barrier, ScissorRect* scissorRect,ViewPort* viewPort);

	// 最初
	void PreDrawOffscreen();

	// 最初
	void PostDrawOffscreen();

	// 2D
	void PreDraw2dOffscreen();

	// 2D
	void PostDraw2dOffscreen();




	
	void AllPostEffect(SceneManager* sceneManager);


	void Update(Camera* camera);

	// レンダーテクスチャ追加
	void AddEffectBlock(const std::string name, PostEffectBlockType type,bool use = true);
	//
	void AddEffectBlocks(std::vector<PostEffectBlock*> effectBlocks);

	//
	RenderTexture* GetEndRenderTexture() { return renderTextureEnd_.get();};

	void ClearPostEffectBlock();

private:

	void PreEnd(RenderTexture* renderTexture);

	void PostEnd(RenderTexture* renderTexture);


private:
	std::unique_ptr <RenderTexture> renderTexture_;
	std::unique_ptr <RenderTexture> renderTextureEnd_;
	
	std::vector<PostEffectBlock*> effectBlocks_;
	
	uint32_t indexCount_ = 0;

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

