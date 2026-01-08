#include "Entity2DManager.h"

void Engine::Entity2DManager::Initialize(DirectXCommon* directXCommon)
{
	directXCommon_ = directXCommon;	// DX共通クラス 
		
	// スプライト共通クラス初期化
	spriteCommon_ = std::make_unique<SpriteCommon>();
	spriteCommon_->Initialize(directXCommon_);

}
