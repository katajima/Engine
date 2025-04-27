#include "BaseUI.h"

#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"

void BaseUI::InitSprite(Sprite* sprite, std::string texFile, Vector2 pos, Vector2 size)
{
	sprite->Initialize( entity2DManager_->GetSpriteCommon(), texFile);
	sprite->SetPosition(pos);
	sprite->SetSize(size);
}
