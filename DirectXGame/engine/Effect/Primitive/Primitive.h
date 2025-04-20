#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<list>
#include<string>
#include<vector>
#include<format>
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include"DirectXGame/engine/Camera/Camera.h"

#include"DirectXGame/engine/Mesh/Mesh.h"
#include"DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/Transfomation/Transfomation.h"


#include "DirectXGame/engine/collider/3d/Collider.h"


#include "DirectXGame/engine/Effect/Primitive/ShapeType/ShapePlane.h"


#include "ShapeParameter.h"


class PrimitiveCommon;
class Primitive
{
public:
	enum class PsoType {
		kDefalt,
		kRingClamp

	};


	enum class ShapeType
	{
		None,			// 形なし(デフォルト)
		Plane,			// 平面
		Triangle,		// 三角形
		Circle,			// 円
		Star,			// 星
		Crescent,		// 三日月
		Ring,			// リング
		Arrow,			// 矢印
		Cross,          // 十字

		Cube,			// 四角形
		Sphere,			// 球
		Cylinder,		// 円柱
		Tube,			// 円筒
		Pyramid,		// 角錐
		Torus,			// トーラス	
		Spring          // ばね
	};
	

	// 形
	void Initialize(PrimitiveCommon* primitiveCommon, ShapeType type, const std::string& tex,const Color color = { 1,1,1,1 }, const std::string& name = "", bool isLine = false);
	// 更新
	void Update();
	// 描画
	void Draw(PsoType type = PsoType::kDefalt);

	void SetCamera(Camera* camera) { camera_ = camera; };

	Mesh* GetMesh() { return mesh.get(); }

	

private:
	void MeshInitialize();

	void MeshUpdate();

	void MeshUpdateImGui();
private:
	
public:
	void SetColor(const Color& color) { material->color = color; }


public:

private:
	// 面
	ShapeParameter::ShapePlane plane;
	ShapeParameter::ShapePlane oPlane;
	// 三角
	ShapeParameter::ShapeTriangle triangle;
	ShapeParameter::ShapeTriangle oTriangle;
	// 円
	ShapeParameter::Circle circle;
	ShapeParameter::Circle oCircle;
	// 立方体
	ShapeParameter::ShapeCube cube;
	ShapeParameter::ShapeCube oCube;
	// 星
	ShapeParameter::Star star;
	ShapeParameter::Star oStar;
	// 三日月
	ShapeParameter::Crescent crescent;
	ShapeParameter::Crescent oCrescent;
	// リング
	ShapeParameter::Ring ring;
	ShapeParameter::Ring oRing;
	// 十字
	ShapeParameter::ShapeCross cross_;
	ShapeParameter::ShapeCross oCross_;
	// ばね
	ShapeParameter::Spring spring;
	ShapeParameter::Spring oSpring;
	// 円柱
	ShapeParameter::Cylinder cylinder;
	ShapeParameter::Cylinder oCylinder;
	// 矢印
	ShapeParameter::ShapeArrow arrow;
	ShapeParameter::ShapeArrow oArrow;
	// 球
	ShapeParameter::ShapeSphere sphere;
	ShapeParameter::ShapeSphere oSphere;
	// 円筒
	ShapeParameter::Tube tube;
	ShapeParameter::Tube oTube;
	// 角錐
	ShapeParameter::Pyramid pyramid;
	ShapeParameter::Pyramid oPyramid;

	ShapeParameter::Torus torus;
	ShapeParameter::Torus oTorus;

	


	
public: //セッター
	void SetName(const std::string str) { name_ = str; };

	void SetStar(ShapeParameter::Star& _star);

public: // ゲッター
	Vector3 GetCubeSize() const { return cube.size; };

private:
	// カメラ
	Camera* camera_ = nullptr;

	bool isLine_ = false;

	std::unique_ptr<Mesh> mesh;
	std::unique_ptr<Material> material;
	std::unique_ptr<Transfomation> transfomation = nullptr;

	// 名前
	std::string name_ = "primitive";

	// 図形タイプ
	ShapeType type_;

	PrimitiveCommon* primitiveCommon_;
	



	float timer_ = 0.0f;
	int count_ = 0;

public:
	Matrix4x4 mat_;
	Transform transform;
};

