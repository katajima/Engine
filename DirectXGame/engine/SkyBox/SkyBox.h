#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/struct/Light.h"
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<string>
#include<vector>
#include<format>
#include<DirectXGame/engine/Material/Material.h>
#include<DirectXGame/engine/Mesh/Mesh.h>



class Entity3DManager;
class SkyBox
{
public:
	// 初期化
	void Initialize(Entity3DManager* entity3DManager,std::string txtueName);

	void Update();

	void Draw();

private:

	std::unique_ptr<Mesh> mesh_;
	std::unique_ptr<Material> material;

private:
	Entity3DManager* entity3DManager_;

};

