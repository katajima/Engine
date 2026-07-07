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

#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"

#include"DirectXGame/engine/struct/Structs3D.h"
#include"DirectXGame/engine/math/MathFunctions.h"


#include"DirectXGame/engine/Mesh/ModelMesh.h"
#include"DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/Transform/Transformation/Transformation.h"
#include "DirectXGame/engine/Animation/UV/UVAnimationComponent.h"



#include "ShapeParameter.h"



namespace Engine {
	// 前方宣言
	class PrimitiveCommon;
	class Camera;
	/// <summary>
	/// プリミティブ基底クラス
	/// </summary>
	class BasePrimitive {
	public:
		~BasePrimitive() = default;

		enum class PsoType {
			kDefault,
			kRingClamp,
			kNoCull,
			kNoCullRingClamp,
			kNoCullWireFrame,

		};
		// 初期化
		void Initialize(PrimitiveCommon* primitiveCommon, const std::string& tex, const Color color = { 1,1,1,1 }, const std::string& name = "no", bool isLine = false);

		// 更新
		void Update(float deltaTime);
		// 描画
		void Draw();
	public:

	public:
		// マテリアル取得
		Material* GetMaterial() { return  mesh->material.get(); }
		// メッシュ取得
		ModelMesh* GetModelMesh() { return mesh.get(); }
		// 描画方法取得
		PsoType GetPsoType() const { return psoType_; }
		// 描画方法設定
		void SetPsoType(PsoType type) { psoType_ = type; }
		// 名前設定
		void SetName(const std::string str) { name_ = str; };
		// 描画準備
		void DrawSetting(PsoType type = PsoType::kDefault);

	private:



		// 固有の更新
		virtual void UniqeUpdate() = 0;

	public:
		// メッシュ初期化
		virtual void MeshInitialize() = 0;
	protected:
		bool isLine_ = false;

		std::unique_ptr<ModelMesh> mesh;

		// 名前
		std::string name_ = "primitive";
		PsoType psoType_ = PsoType::kDefault;

		UVAnimetionData aimetion_{};
	private:
		PrimitiveCommon* primitiveCommon = nullptr;
	};


	// 平面
	class PlanePrimitive :public BasePrimitive {
	public:
		// データ取得
		ShapeParameter::ShapePlane& Data() { return plane; };
		// メッシュ初期化
		void MeshInitialize() override { plane.Create(mesh.get()); };
		// 逆法線
		void InvNormal();

	private:
		// 固有の更新
		void UniqeUpdate() override;
	private:
		ShapeParameter::ShapePlane plane;
		ShapeParameter::ShapePlane prePlane;
	};

	// 三角面
	class TrianglePrimitive :public BasePrimitive {
	public:
		// データ取得
		ShapeParameter::ShapeTriangle& Data() { return triangle; };
		// メッシュ初期化
		void MeshInitialize() override { triangle.Create(mesh.get()); };
	private:
		// 固有の更新
		void UniqeUpdate() override;
	private:
		ShapeParameter::ShapeTriangle triangle;
		ShapeParameter::ShapeTriangle preTriangle;
	};

	// 円
	class CirclePrimitive :public BasePrimitive {
	public:
		// データ取得
		ShapeParameter::Circle& Data() { return circle; };
		// メッシュ初期化
		void MeshInitialize() override { circle.Create(mesh.get()); };
	private:
		// 固有の更新
		void UniqeUpdate() override;
	private:
		ShapeParameter::Circle circle;
		ShapeParameter::Circle preCircle;
	};

	// 立方体
	class CubePrimitive :public BasePrimitive {
	public:
		// データ取得
		ShapeParameter::ShapeCube& Data() { return cube; };
		// メッシュ初期化
		void MeshInitialize() override { cube.Create(mesh.get()); };
	private:
		// 固有の更新
		void UniqeUpdate() override;
	private:
		ShapeParameter::ShapeCube cube;
		ShapeParameter::ShapeCube preCube;
	};

	// 星
	class StarPrimitive :public BasePrimitive {
	public:
		// データ取得
		ShapeParameter::Star& Data() { return star; };
		// メッシュ初期化
		void MeshInitialize() override { star.Create(mesh.get()); };
	private:
		// 固有の更新
		void UniqeUpdate() override;
	private:
		ShapeParameter::Star star;
		ShapeParameter::Star preStar;
	};

	// 三日月
	class CrescentPrimitive :public BasePrimitive {
	public:
		// データ取得
		ShapeParameter::Crescent& Data() { return crescent; };
		// メッシュ初期化
		void MeshInitialize() override { crescent.Create(mesh.get()); };
	private:
		// 固有の更新
		void UniqeUpdate() override;
	private:
		ShapeParameter::Crescent crescent;
		ShapeParameter::Crescent preCrescent;
	};

	// リング
	class RingPrimitive :public BasePrimitive {
	public:
		// データ取得
		ShapeParameter::Ring& Data() { return ring; };
		// メッシュ初期化
		void MeshInitialize() override { ring.Create(mesh.get()); };
	private:
		// 固有の更新
		void UniqeUpdate() override;
	private:
		ShapeParameter::Ring ring;
		ShapeParameter::Ring preRing;
	};

	// 十字
	class CrossPrimitive :public BasePrimitive {
	public:
		// データ取得
		ShapeParameter::ShapeCross& Data() { return cross; };
		// メッシュ初期化
		void MeshInitialize() override { cross.Create(mesh.get()); };
	private:
		// 固有の更新
		void UniqeUpdate() override;
	private:
		ShapeParameter::ShapeCross cross;
		ShapeParameter::ShapeCross preCross;
	};

	// 円柱
	class CylinderPrimitive :public BasePrimitive {
	public:
		// データ取得
		ShapeParameter::Cylinder& Data() { return cylinder; };
		// メッシュ初期化
		void MeshInitialize() override { cylinder.Create(mesh.get()); };
	private:
		// 固有の更新
		void UniqeUpdate() override;
	private:
		ShapeParameter::Cylinder cylinder;
		ShapeParameter::Cylinder preCylinder;
	};

	// 球
	class SpherePrimitive :public BasePrimitive {
	public:
		// データ取得
		ShapeParameter::ShapeSphere& Data() { return sphere; };
		// メッシュ初期化
		void MeshInitialize() override { sphere.Create(mesh.get()); };
	private:
		// 固有の更新
		void UniqeUpdate() override;
	private:
		ShapeParameter::ShapeSphere sphere;
		ShapeParameter::ShapeSphere preSphere;
	};

	// 矢印
	class ArrowPrimitive :public BasePrimitive {
	public:
		// データ取得
		ShapeParameter::ShapeArrow& Data() { return arrow; };
		// メッシュ初期化
		void MeshInitialize() override { arrow.Create(mesh.get()); };
	private:
		// 固有の更新
		void UniqeUpdate() override;
	private:
		ShapeParameter::ShapeArrow arrow;
		ShapeParameter::ShapeArrow preArrow;
	};

	// 筒
	class TubePrimitive :public BasePrimitive {
	public:
		// データ取得
		ShapeParameter::Tube& Data() { return tube; };
		// メッシュ初期化
		void MeshInitialize() override { tube.Create(mesh.get()); };
	private:
		// 固有の更新
		void UniqeUpdate() override;
	private:
		ShapeParameter::Tube tube;
		ShapeParameter::Tube preTube;
	};

	// 角錐
	class PyramidPrimitive :public BasePrimitive {
	public:
		// データ取得
		ShapeParameter::Pyramid& Data() { return pyramid; };
		// メッシュ初期化
		void MeshInitialize() override { pyramid.Create(mesh.get()); };
	private:
		// 固有の更新
		void UniqeUpdate() override;
	private:
		ShapeParameter::Pyramid pyramid;
		ShapeParameter::Pyramid prePyramid;
	};

	// トーラス
	class TorusPrimitive :public BasePrimitive {
	public:
		// データ取得
		ShapeParameter::Torus& Data() { return torus; };
		// メッシュ初期化
		void MeshInitialize() override { torus.Create(mesh.get()); };
	private:
		// 固有の更新
		void UniqeUpdate() override;
	private:
		ShapeParameter::Torus torus;
		ShapeParameter::Torus preTorus;
	};
}
