#include "BaseSprite.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void BaseSprite::Init(Entity2DManager* entity2DManager, std::string name, std::string textureName)
{
	sprite_ = std::make_unique<Sprite>();									//
	sprite_->Initialize(entity2DManager->GetSpriteCommon(), textureName);	// スプライト初期化
	sprite_->SetName(name);													// 名前設定
	colorComponent_ = std::make_unique<ColorComponent>();					// 色コンポーネント初期化
}
