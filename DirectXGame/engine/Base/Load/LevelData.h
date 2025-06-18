#pragma once
#include "DirectXGame/engine/math/MathFanctions.h"
#include "DirectXGame/engine/struct/Structs3D.h"
#include <string>

class LevelData
{
public:
	struct ObjectData
	{
		std::string fileName;
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
	};
	std::vector<ObjectData> objects;

private:

};
