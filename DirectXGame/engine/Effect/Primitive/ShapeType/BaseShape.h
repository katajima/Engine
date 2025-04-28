#pragma once
#include"DirectXGame/engine/Mesh/Mesh.h"
#include"DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/Transfomation/Transfomation.h"

class PrimitiveCommon;
class BaseShape {
public:

	virtual void Initialize(PrimitiveCommon* primitiveCommon) = 0;

	virtual void Update() = 0;

	
private:

	virtual void UpdateImGui() = 0;

	virtual void UpdateMesh() = 0;

protected:
	std::unique_ptr<Mesh> mesh_;         // めっしゅ
};

