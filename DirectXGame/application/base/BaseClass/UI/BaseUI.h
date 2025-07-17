#pragma once
#include"DirectXGame/engine/2d/Sprite.h"

class Entity2DManager;
class BaseUI
{
public:
	virtual void Initialize(Entity2DManager* entity2DManager) = 0;

	// 毎フレーム更新
	virtual void Update() = 0;

	// 描画
	virtual void Draw() = 0;

protected:

	void InitSprite(Sprite* sprite,std::string texFile,Vector2 pos,Vector2 size);



protected:
	Entity2DManager* entity2DManager_;

	std::vector<std::unique_ptr<Sprite>> sprite_;



private:



};

