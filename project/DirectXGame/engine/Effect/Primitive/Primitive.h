#pragma once
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cstdint>
#include<wrl.h>
#include<list>
#include<string>
#include<vector>
#include<format>
#include<variant>
#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include"DirectXGame/engine/Camera/Camera.h"

#include"DirectXGame/engine/Mesh/ModelMesh.h"
#include"DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/Transform/Transfomation/Transfomation.h"
#include "DirectXGame/engine/Animation/UV/UVAnimationComponent.h"

#include "DirectXGame/engine/collider/3d/Collider.h"
#include "ShapeParameter.h"



class PrimitiveCommon;
class BasePrimitive {
public:
	~BasePrimitive() = default;

	enum class PsoType {
		kDefalt,
		kRingClamp,
		kNoCull,
		kNoCullRingClamp,
		kNoCullWireFrame,

	};

	void Initialize(PrimitiveCommon* primitiveCommon, const std::string& tex, const Color color = { 1,1,1,1 }, const std::string& name = "no", bool isLine = false);

	// 更新
	void Update(float deltaTime);

	void Draw();
public:

public:
	Material* GetMaterial() { return  mesh->material.get(); }

	ModelMesh* GetModelMesh() { return mesh.get(); }

	PsoType GetPsoType() const { return psoType_; }

	void SetPsoType(PsoType type) { psoType_ = type; }

	void SetName(const std::string str) { name_ = str; };

	void DrawSetting(PsoType type = PsoType::kDefalt);

private:




	virtual void UniqeUpdate() = 0;

public:
	virtual void MeshInitialize() = 0;
protected:
	bool isLine_ = false;

	std::unique_ptr<ModelMesh> mesh;

	// 名前
	std::string name_ = "primitive";
	PsoType psoType_ = PsoType::kDefalt;

	UVAnimetionData aimetion_{};
private:
	PrimitiveCommon* primitiveCommon_;
};


// 平面
class PlanePrimitive :public BasePrimitive {
public:
	ShapeParameter::ShapePlane& Data() { return plane; };

	void MeshInitialize() override { plane.Create(mesh.get()); };

	void InvNormal();

private:
	void UniqeUpdate() override;
private:
	ShapeParameter::ShapePlane plane;
	ShapeParameter::ShapePlane prePlane;
};

// 三角面
class TrianglePrimitive :public BasePrimitive {
public:
	ShapeParameter::ShapeTriangle& Data() { return triangle; };
	void MeshInitialize() override { triangle.Create(mesh.get()); };
private:
	void UniqeUpdate() override;
private:
	ShapeParameter::ShapeTriangle triangle;
	ShapeParameter::ShapeTriangle preTriangle;
};

// 円
class CirclePrimitive :public BasePrimitive {
public:
	ShapeParameter::Circle& Data() { return circle; };
	void MeshInitialize() override { circle.Create(mesh.get()); };
private:

	void UniqeUpdate() override;
private:
	ShapeParameter::Circle circle;
	ShapeParameter::Circle preCircle;
};

// 立方体
class CubePrimitive :public BasePrimitive {
public:
	ShapeParameter::ShapeCube& Data() { return cube; };
	void MeshInitialize() override { cube.Create(mesh.get()); };
private:

	void UniqeUpdate() override;
private:
	ShapeParameter::ShapeCube cube;
	ShapeParameter::ShapeCube preCube;
};

// 星
class StarPrimitive :public BasePrimitive {
public:
	ShapeParameter::Star& Data() { return star; };
	void MeshInitialize() override { star.Create(mesh.get()); };
private:

	void UniqeUpdate() override;
private:
	ShapeParameter::Star star;
	ShapeParameter::Star preStar;
};

// 三日月
class CrescentPrimitive :public BasePrimitive {
public:
	ShapeParameter::Crescent& Data() { return crescent; };
	void MeshInitialize() override { crescent.Create(mesh.get()); };
private:

	void UniqeUpdate() override;
private:
	ShapeParameter::Crescent crescent;
	ShapeParameter::Crescent preCrescent;
};

// リング
class RingPrimitive :public BasePrimitive {
public:
	ShapeParameter::Ring& Data() { return ring; };
	void MeshInitialize() override { ring.Create(mesh.get()); };
private:

	void UniqeUpdate() override;
private:
	ShapeParameter::Ring ring;
	ShapeParameter::Ring preRing;
};

// 十字
class CrossPrimitive :public BasePrimitive {
public:
	ShapeParameter::ShapeCross& Data() { return cross; };
	void MeshInitialize() override { cross.Create(mesh.get()); };
private:

	void UniqeUpdate() override;
private:
	ShapeParameter::ShapeCross cross;
	ShapeParameter::ShapeCross preCross;
};

// 円柱
class CylinderPrimitive :public BasePrimitive {
public:
	ShapeParameter::Cylinder& Data() { return cylinder; };
	void MeshInitialize() override { cylinder.Create(mesh.get()); };
private:

	void UniqeUpdate() override;
private:
	ShapeParameter::Cylinder cylinder;
	ShapeParameter::Cylinder preCylinder;
};

// 球
class SpherePrimitive :public BasePrimitive {
public:
	ShapeParameter::ShapeSphere& Data() { return sphere; };
	void MeshInitialize() override { sphere.Create(mesh.get()); };
private:

	void UniqeUpdate() override;
private:
	ShapeParameter::ShapeSphere sphere;
	ShapeParameter::ShapeSphere preSphere;
};

// 矢印
class ArrowPrimitive :public BasePrimitive {
public:
	ShapeParameter::ShapeArrow& Data() { return arrow; };
	void MeshInitialize() override { arrow.Create(mesh.get()); };
private:

	void UniqeUpdate() override;
private:
	ShapeParameter::ShapeArrow arrow;
	ShapeParameter::ShapeArrow preArrow;
};

// 筒
class TubePrimitive :public BasePrimitive {
public:
	ShapeParameter::Tube& Data() { return tube; };
	void MeshInitialize() override { tube.Create(mesh.get()); };
private:

	void UniqeUpdate() override;
private:
	ShapeParameter::Tube tube;
	ShapeParameter::Tube preTube;
};

// 角錐
class PyramidPrimitive :public BasePrimitive {
public:
	ShapeParameter::Pyramid& Data() { return pyramid; };
	void MeshInitialize() override { pyramid.Create(mesh.get()); };
private:

	void UniqeUpdate() override;
private:
	ShapeParameter::Pyramid pyramid;
	ShapeParameter::Pyramid prePyramid;
};

// トーラス
class TorusPrimitive :public BasePrimitive {
public:
	ShapeParameter::Torus& Data() { return torus; };
	void MeshInitialize() override { torus.Create(mesh.get()); };
private:
	void UniqeUpdate() override;
private:
	ShapeParameter::Torus torus;
	ShapeParameter::Torus preTorus;
};

