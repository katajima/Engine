#pragma once

// C++
#include"memory"

// engine
#include"DirectXGame/engine/2d/SpriteCommon.h"

class DirectXCommon;
class Entity2DManager
{
public:
	// 初期化
	void Initialize(DirectXCommon* directXCommon);


	SpriteCommon* GetSpriteCommon() {return spriteCommon_.get();}

private:
	// DirectX
	DirectXCommon* directXCommon_;

	std::unique_ptr<SpriteCommon> spriteCommon_;
};

