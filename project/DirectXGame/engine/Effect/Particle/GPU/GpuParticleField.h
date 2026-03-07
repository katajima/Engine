#pragma once
#include "GpuParticleData.h"


namespace Engine {
	// 前方宣言
	class LineCommon;
	class DirectXCommon;

	/// <summary>
	/// GPUパーティクルフィールド
	/// </summary>
	class GpuParticleField
	{
	public:
		// 初期化
		void Init(DirectXCommon* dxCommon, LineCommon* lineCommon, std::string name);
		// ImGui更新
		void UpdateImgui();
		// 更新
		void Update();

	private:
		// パーティクル影響場所
		ConstantBuffer<EffectFieldCS> cbEffectFieldResource_;


		std::string name_;
	private:
		LineCommon* lineCommon = nullptr;
		DirectXCommon* dxCommon = nullptr;
	};
}

