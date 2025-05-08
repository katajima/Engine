#pragma once
#include "DirectXGame/engine/WorldTransform/WorldTransform.h"

struct Entity3D
{
	WorldTransform transform_;
	std::string name = "";
	bool isDraw = false;
};
