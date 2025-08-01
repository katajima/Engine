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


#include "DirectXGame/engine/collider/3d/Collider.h"

#include "ShapeParameter.h"


//項目
using ShapeParameters = std::variant<ShapeParameter::ShapePlane, ShapeParameter::ShapeTriangle, ShapeParameter::Circle, ShapeParameter::ShapeCube,
	ShapeParameter::Star, ShapeParameter::Crescent, ShapeParameter::Ring, ShapeParameter::ShapeCross, ShapeParameter::Cylinder, ShapeParameter::ShapeArrow,
	ShapeParameter::ShapeSphere, ShapeParameter::Tube, ShapeParameter::Pyramid, ShapeParameter::Torus>;

class PrimitiveCommon;
class Primitive
{
public:

public:
	enum class PsoType {
		kDefalt,
		kRingClamp,
		kNoCull,
		kNoCullRingClamp,
		kNoCullWireFrame,

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
	template <typename T>
	void Initialize(PrimitiveCommon* primitiveCommon, ShapeType type, const ShapeParameters& parameter, const std::string& tex, const Color color = { 1,1,1,1 }, const std::string& name = "", bool isLine = false)
	{
		primitiveCommon_ = primitiveCommon;

		std::visit([&](auto&& shapeParam) {
			using ParamType = std::decay_t<decltype(shapeParam)>;

			if constexpr (std::is_same_v<ParamType, ShapeParameter::ShapePlane>) {
				plane = shapeParam;
			}
			else if constexpr (std::is_same_v<ParamType, ShapeParameter::ShapeTriangle>) {
				triangle = shapeParam;
			}
			else if constexpr (std::is_same_v<ParamType, ShapeParameter::Circle>) {
				circle = shapeParam;
			}
			else if constexpr (std::is_same_v<ParamType, ShapeParameter::ShapeCube>) {
				cube = shapeParam;
			}
			else if constexpr (std::is_same_v<ParamType, ShapeParameter::Star>) {
				star = shapeParam;
			}
			else if constexpr (std::is_same_v<ParamType, ShapeParameter::Crescent>) {
				crescent = shapeParam;
			}
			else if constexpr (std::is_same_v<ParamType, ShapeParameter::Ring>) {
				ring = shapeParam;
			}
			else if constexpr (std::is_same_v<ParamType, ShapeParameter::ShapeCross>) {
				cross_ = shapeParam;
			}
			else if constexpr (std::is_same_v<ParamType, ShapeParameter::Cylinder>) {
				cylinder = shapeParam;
			}
			else if constexpr (std::is_same_v<ParamType, ShapeParameter::ShapeArrow>) {
				arrow = shapeParam;
			}
			else if constexpr (std::is_same_v<ParamType, ShapeParameter::ShapeSphere>) {
				sphere = shapeParam;
			}
			else if constexpr (std::is_same_v<ParamType, ShapeParameter::Tube>) {
				tube = shapeParam;
			}
			else if constexpr (std::is_same_v<ParamType, ShapeParameter::Pyramid>) {
				pyramid = shapeParam;
			}
			else if constexpr (std::is_same_v<ParamType, ShapeParameter::Torus>) {
				torus = shapeParam;
			}
			}, parameter);

		type_ = type;

		isLine_ = isLine;

		Init(tex,color,name);

	};

	// 更新
	void Update();
	// 描画
	void Draw();

	void DrawSetting(PsoType type = PsoType::kDefalt);

	Material* GetMaterial() { return material.get(); }

	ModelMesh* GetMesh() { return mesh.get(); }

	void MeshUpdateImGui();

private:

	void Init(const std::string& tex, const Color color, const std::string& name);

private:
	void MeshInitialize();

	void MeshUpdate();

	
private:

public:
	void SetColor(const Color& color) { material->color = color; }

	PsoType GetPsoType() const { return psoType_; }

	void SetPsoType(PsoType type) { psoType_ = type; }

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
	// トーラス
	ShapeParameter::Torus torus;
	ShapeParameter::Torus oTorus;

public: //セッター
	void SetName(const std::string str) { name_ = str; };

private:
	bool isLine_ = false;

	std::unique_ptr<ModelMesh> mesh;
	std::unique_ptr<Material> material;
	
	// 名前
	std::string name_ = "primitive";

	// 図形タイプ
	ShapeType type_;

	PrimitiveCommon* primitiveCommon_;

	PsoType psoType_ = PsoType::kDefalt;

	struct UVAnimetion {
		bool isScaleX = false;
		bool isScaleY = false;
		Vector2 maxCount{ 10,10 };
		Vector2 speed{0.2f,0.2f};

		bool isRotateX = false;
		bool isRotateY = false;
		Vector2 maxRotate{ 10,10 };
		Vector2 rotateSpeed{0.2f,0.2f};

	};
	UVAnimetion aimetion_{};

};

