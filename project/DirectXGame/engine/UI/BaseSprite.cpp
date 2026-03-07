#include "BaseSprite.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

void Engine::BaseSprite::Init(EntityManager* entityManager, std::string name, std::string textureName)
{
	sprite_ = std::make_unique<Sprite>();									//
	sprite_->Initialize(entityManager->GetSpriteCommon(), textureName);	// スプライト初期化
	sprite_->SetName(name);													// 名前設定
	colorComponent_ = std::make_unique<ColorComponent>();					// 色コンポーネント初期化
}
