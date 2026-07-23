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
		/// <summary>
		/// 初期化
		/// </summary>
		void Init(DirectXCommon* dxCommon, LineCommon* lineCommon, std::string name);
		/// <summary>
		/// ImGui更新
		/// </summary>
		void UpdateImgui();
		/// <summary>
		/// 更新
		/// </summary>
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

