#pragma once
#include"DirectXGame/engine/Material/Material.h"


struct UVAnimetionData {
	bool isScaleX = false;
	bool isScaleY = false;
	Vector2 maxCount{ 10,10 };
	Vector2 speed{ 0.2f,0.2f };

	bool isRotateX = false;
	bool isRotateY = false;
	Vector2 maxRotate{ 10,10 };
	Vector2 rotateSpeed{ 0.2f,0.2f };

};


class UVAnimationComponent
{
public:
	
	void Init(Material* mateirial);

	void Update(float deltaTime);



public:
	// UVアニメーションデータ取得
	UVAnimetionData& GetUVAnimetionData() { return uvAnimationData_; }
	// マテリアルセット
	void Setmaterial(Material* mateiria) { mateirial_ = mateiria; };
private:
	// 
	UVAnimetionData uvAnimationData_;
private:
	Material* mateirial_;
};

