#pragma once

#include <memory>
#include <string>
#include <vector>

#include "DirectXGame/engine/DirectX/RenderTexture/RenderTexture.h"

namespace Engine {
	class DXGIDevice;
	class Command;
	class SrvManager;
	class RtvManager;
	class RenderingCommon;
	class DepthStencil;
	class Barrier;
	class ScissorRect;
	class ViewPort;
	class Camera;

	// 1回のポストエフェクト描画を表す単位。
	// 入力RenderTextureを読み込み、自分のRenderTextureへ書き出す。
/// <summary>
/// PostEffectPassを管理・実装するクラス。
/// </summary>
	class PostEffectPass {
	public:
		void Initialize(DXGIDevice* dxgiDevice, Command* command, SrvManager* srvManager,
			RtvManager* rtvManager, RenderingCommon* renderingCommon,
			DepthStencil* depthStencil, Barrier* barrier,
			ScissorRect* scissorRect, ViewPort* viewPort,
			const std::string& name, PostEffectType type);

		void Update(Camera* camera);
		void Draw(RenderTexture* inputTexture);

		RenderTexture* GetRenderTexture() { return renderTexture_.get(); }
		PostEffectData* GetPostEffectData();

		bool GetUse() const { return use_; }
		void SetUse(bool use) { use_ = use; }

		uint32_t GetIndex() const { return index_; }
		void SetIndex(uint32_t index) { index_ = index; }

		// BloomCombineなど、通常の直前入力とは別に参照したいパス。
		PostEffectPass* GetOtherInputPass() const { return otherInputPass_; }
		void SetOtherInputPass(PostEffectPass* pass) { otherInputPass_ = pass; }

		const std::string& GetName() const { return name_; }
		PostEffectType GetType() const { return type_; }

	private:
		void PreDraw();
		void PostDraw();

	private:
		std::unique_ptr<RenderTexture> renderTexture_;
		std::string name_;
		PostEffectType type_ = PostEffectType::kCopy;
		bool use_ = true;
		uint32_t index_ = 0;
		PostEffectPass* otherInputPass_ = nullptr;

		Command* command_ = nullptr;
		DepthStencil* depthStencil_ = nullptr;
		Barrier* barrier_ = nullptr;
		ScissorRect* scissorRect_ = nullptr;
		ViewPort* viewPort_ = nullptr;
	};

	// カメラごとのポストエフェクト列。
	// Passを順番に実行し、最後の出力を次の描画先へ渡す。
/// <summary>
/// PostEffectPipelineを管理・実装するクラス。
/// </summary>
	class PostEffectPipeline {
	public:
		void Initialize(DXGIDevice* dxgiDevice, Command* command, SrvManager* srvManager,
			RtvManager* rtvManager, RenderingCommon* renderingCommon,
			DepthStencil* depthStencil, Barrier* barrier,
			ScissorRect* scissorRect, ViewPort* viewPort);

		PostEffectPass* AddPass(const std::string& name, PostEffectType type, bool use = true);
		PostEffectPass* AddEffect(const std::string& name, PostEffectBlockType type, bool use = true);
		void AddBloom(const std::string& name, bool use = true);

		void Clear();
		void Update(Camera* camera);
		RenderTexture* Execute(RenderTexture* sourceTexture);

		PostEffectPass* GetPass(size_t index);
		PostEffectPass* FindPass(const std::string& name);
		std::vector<PostEffectPass*> GetPasses();

		bool Empty() const { return passes_.empty(); }

	private:
		PostEffectPass* CreatePass(const std::string& name, PostEffectType type, bool use);

	private:
		std::vector<std::unique_ptr<PostEffectPass>> passes_;

		DXGIDevice* dxgiDevice_ = nullptr;
		Command* command_ = nullptr;
		SrvManager* srvManager_ = nullptr;
		RtvManager* rtvManager_ = nullptr;
		RenderingCommon* renderingCommon_ = nullptr;
		DepthStencil* depthStencil_ = nullptr;
		Barrier* barrier_ = nullptr;
		ScissorRect* scissorRect_ = nullptr;
		ViewPort* viewPort_ = nullptr;
	};
}
