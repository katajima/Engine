#pragma once
#include "DirectXGame/engine/Mesh/ModelMesh.h"
#include "DirectXGame/engine/3d/Model/ModelData.h"
#include"DirectXGame/engine/3d/Model/Model.h"


// 描画するときの映り方を指定する
enum class ObjectRasterizerType {
	UvInterpolation_MODE_SOLID_BACK,
	NoUvInterpolation_MODE_SOLID_BACK,
	UvInterpolation_MODE_WIREFRAME_BACK,
	NoUvInterpolation_MODE_WIREFRAME_BACK,

	UvInterpolation_MODE_SOLID_NONE,
	NoUvInterpolation_MODE_SOLID_NONE,
	UvInterpolation_MODE_WIREFRAME_NONE,
	NoUvInterpolation_MODE_WIREFRAME_NONE,
};

// 描画する順番
enum class ObjectDrawType {
	kOpaque,		// 不透明
	kTranslucent01,	// 半透明最前
	kTranslucent02,	// 半透明中
	kTranslucent03,	// 半透明最後
};


class RenderComponent 
{
public:
	
	void Init();


	void Update();

	void Draw();



private:

};

