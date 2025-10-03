#pragma once
#include "DirectXGame/engine/WorldTransform/WorldTransform.h"
#include "DirectXGame/engine/Material/Material.h"

struct Entity3D
{
	WorldTransform* transform_;
	Material* material_;

	std::string name = "";
	bool isDraw = false;
};
