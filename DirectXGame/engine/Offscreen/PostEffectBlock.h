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
		DepthStencil* depthStencil, Barrier* barrier, ScissorRect* scissorRect, ViewPort* viewPort,
		const std::string name, PostEffectBlockType type);

	void Update(Camera* camera);

	// レンダーテクスチャ追加
	void AddRenderTexture(const std::string name, PostEffectType type);


	// テクスチャ書き込み
	void DrawRenderTexture(RenderTexture* renderTextureRenderTreget, RenderTexture* renderTexturePixelSheder);

	// エフェクト
	void DrawEffectBlock(RenderTexture* inputTexture);

	// 繋げる
	void ConnectBlock(RenderTexture* other);


	RenderTexture* GetRenderTextures(int i) {
		if (renderTextures_.empty()) return nullptr;
		return renderTextures_[i].get();
	}
	// 最初
	RenderTexture* GetFirstRenderTexture() {
		if (renderTextures_.empty()) return nullptr;
		return renderTextures_.front().get();
	};
	// 最後
	RenderTexture* GetEndRenderTexture() {
		if (renderTextures_.empty()) return nullptr;
		return renderTextures_.back().get();
	};

	// 使用するか取得
	bool GetUse() const { return use_; }
	// 使用するか設定
	void SetUse(bool use) { use_ = use; }
	// 順番取得
	uint32_t GetIndex() const { return index_; }
	// 順番設定
	void SetIndex(uint32_t index) { index_ = index; }

private:
	// レンダーテクスチャ描画前処理
	void PreDraw(RenderTexture* renderTexture);
	// レンダーテクスチャ描画後処理
	void PostDraw(RenderTexture* renderTexture);

private:

	// レンダーテクスチャたち
	std::vector<std::unique_ptr<RenderTexture>> renderTextures_;

	// 名前
	std::string name_ = "none";
private:
	// 使うか
	bool use_ = true;
	// 順番
	uint32_t index_ = 0;


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
