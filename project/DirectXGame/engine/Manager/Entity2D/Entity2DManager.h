#pragma once

// C++
#include"memory"

// engine
#include"DirectXGame/engine/2d/SpriteCommon.h"

namespace Engine {
	// 前方宣言
	class DirectXCommon;

	/// <summary>
	/// エンティティ2dマネージャー
	/// </summary>
	class Entity2DManager
	{
	public:
		// 初期化
		void Initialize(DirectXCommon* directXCommon);

		// スプライト共通クラス取得
		SpriteCommon* GetSpriteCommon() { return spriteCommon_.get(); }

	private:
		// DirectX
		DirectXCommon* directXCommon_;

		std::unique_ptr<SpriteCommon> spriteCommon_;
	};
}
