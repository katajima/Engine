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
		/// <summary>
		/// 派生プリミティブをBasePrimitive経由で破棄しても、派生側まで正しく解放する。
		/// </summary>
		virtual ~BasePrimitive() = default;

		enum class PsoType {
			kDefault,
			kRingClamp,
			kNoCull,
			kNoCullRingClamp,
			kNoCullWireFrame,

		};
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(PrimitiveCommon* primitiveCommon, const std::string& tex, const Color color = { 1,1,1,1 }, const std::string& name = "no", bool isLine = false);

		/// <summary>
		/// 更新
		/// </summary>
		void Update(float deltaTime);
		/// <summary>
		/// 描画
		/// </summary>
		void Draw();
	public:

	public:
		/// <summary>
		/// マテリアル取得
		/// </summary>
		Material* GetMaterial() { return  mesh->material.get(); }
		/// <summary>
		/// メッシュ取得
		/// </summary>
		ModelMesh* GetModelMesh() { return mesh.get(); }
		/// <summary>
		/// 描画方法取得
		/// </summary>
		PsoType GetPsoType() const { return psoType_; }
		/// <summary>
		/// 描画方法設定
		/// </summary>
		void SetPsoType(PsoType type) { psoType_ = type; }
		/// <summary>
		/// 名前設定
		/// </summary>
		void SetName(const std::string str) { name_ = str; };
		/// <summary>
		/// 描画準備
		/// </summary>
		void DrawSetting(PsoType type = PsoType::kDefault);

	private:



		/// <summary>
		/// 固有の更新
		/// </summary>
		virtual void UniqueUpdate() = 0;

	public:
		/// <summary>
		/// メッシュ初期化
		/// </summary>
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
/// <summary>
/// PlanePrimitiveを管理・実装するクラス。
/// </summary>
	class PlanePrimitive :public BasePrimitive {
	public:
		/// <summary>
		/// データ取得
		/// </summary>
		ShapeParameter::ShapePlane& Data() { return plane; };
		/// <summary>
		/// メッシュ初期化
		/// </summary>
		void MeshInitialize() override { plane.Create(mesh.get()); };
		/// <summary>
		/// 逆法線
		/// </summary>
		void InvNormal();

	private:
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UniqueUpdate() override;
	private:
		ShapeParameter::ShapePlane plane;
		ShapeParameter::ShapePlane prePlane;
	};

	// 三角面
/// <summary>
/// TrianglePrimitiveを管理・実装するクラス。
/// </summary>
	class TrianglePrimitive :public BasePrimitive {
	public:
		/// <summary>
		/// データ取得
		/// </summary>
		ShapeParameter::ShapeTriangle& Data() { return triangle; };
		/// <summary>
		/// メッシュ初期化
		/// </summary>
		void MeshInitialize() override { triangle.Create(mesh.get()); };
	private:
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UniqueUpdate() override;
	private:
		ShapeParameter::ShapeTriangle triangle;
		ShapeParameter::ShapeTriangle preTriangle;
	};

	// 円
/// <summary>
/// CirclePrimitiveを管理・実装するクラス。
/// </summary>
	class CirclePrimitive :public BasePrimitive {
	public:
		/// <summary>
		/// データ取得
		/// </summary>
		ShapeParameter::Circle& Data() { return circle; };
		/// <summary>
		/// メッシュ初期化
		/// </summary>
		void MeshInitialize() override { circle.Create(mesh.get()); };
	private:
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UniqueUpdate() override;
	private:
		ShapeParameter::Circle circle;
		ShapeParameter::Circle preCircle;
	};

	// 立方体
/// <summary>
/// CubePrimitiveを管理・実装するクラス。
/// </summary>
	class CubePrimitive :public BasePrimitive {
	public:
		/// <summary>
		/// データ取得
		/// </summary>
		ShapeParameter::ShapeCube& Data() { return cube; };
		/// <summary>
		/// メッシュ初期化
		/// </summary>
		void MeshInitialize() override { cube.Create(mesh.get()); };
	private:
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UniqueUpdate() override;
	private:
		ShapeParameter::ShapeCube cube;
		ShapeParameter::ShapeCube preCube;
	};

	// 星
/// <summary>
/// StarPrimitiveを管理・実装するクラス。
/// </summary>
	class StarPrimitive :public BasePrimitive {
	public:
		/// <summary>
		/// データ取得
		/// </summary>
		ShapeParameter::Star& Data() { return star; };
		/// <summary>
		/// メッシュ初期化
		/// </summary>
		void MeshInitialize() override { star.Create(mesh.get()); };
	private:
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UniqueUpdate() override;
	private:
		ShapeParameter::Star star;
		ShapeParameter::Star preStar;
	};

	// 三日月
/// <summary>
/// CrescentPrimitiveを管理・実装するクラス。
/// </summary>
	class CrescentPrimitive :public BasePrimitive {
	public:
		/// <summary>
		/// データ取得
		/// </summary>
		ShapeParameter::Crescent& Data() { return crescent; };
		/// <summary>
		/// メッシュ初期化
		/// </summary>
		void MeshInitialize() override { crescent.Create(mesh.get()); };
	private:
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UniqueUpdate() override;
	private:
		ShapeParameter::Crescent crescent;
		ShapeParameter::Crescent preCrescent;
	};

	// リング
/// <summary>
/// RingPrimitiveを管理・実装するクラス。
/// </summary>
	class RingPrimitive :public BasePrimitive {
	public:
		/// <summary>
		/// データ取得
		/// </summary>
		ShapeParameter::Ring& Data() { return ring; };
		/// <summary>
		/// メッシュ初期化
		/// </summary>
		void MeshInitialize() override { ring.Create(mesh.get()); };
	private:
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UniqueUpdate() override;
	private:
		ShapeParameter::Ring ring;
		ShapeParameter::Ring preRing;
	};

	// 十字
/// <summary>
/// CrossPrimitiveを管理・実装するクラス。
/// </summary>
	class CrossPrimitive :public BasePrimitive {
	public:
		/// <summary>
		/// データ取得
		/// </summary>
		ShapeParameter::ShapeCross& Data() { return cross; };
		/// <summary>
		/// メッシュ初期化
		/// </summary>
		void MeshInitialize() override { cross.Create(mesh.get()); };
	private:
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UniqueUpdate() override;
	private:
		ShapeParameter::ShapeCross cross;
		ShapeParameter::ShapeCross preCross;
	};

	// 円柱
/// <summary>
/// CylinderPrimitiveを管理・実装するクラス。
/// </summary>
	class CylinderPrimitive :public BasePrimitive {
	public:
		/// <summary>
		/// データ取得
		/// </summary>
		ShapeParameter::Cylinder& Data() { return cylinder; };
		/// <summary>
		/// メッシュ初期化
		/// </summary>
		void MeshInitialize() override { cylinder.Create(mesh.get()); };
	private:
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UniqueUpdate() override;
	private:
		ShapeParameter::Cylinder cylinder;
		ShapeParameter::Cylinder preCylinder;
	};

	// 球
/// <summary>
/// SpherePrimitiveを管理・実装するクラス。
/// </summary>
	class SpherePrimitive :public BasePrimitive {
	public:
		/// <summary>
		/// データ取得
		/// </summary>
		ShapeParameter::ShapeSphere& Data() { return sphere; };
		/// <summary>
		/// メッシュ初期化
		/// </summary>
		void MeshInitialize() override { sphere.Create(mesh.get()); };
	private:
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UniqueUpdate() override;
	private:
		ShapeParameter::ShapeSphere sphere;
		ShapeParameter::ShapeSphere preSphere;
	};

	// 矢印
/// <summary>
/// ArrowPrimitiveを管理・実装するクラス。
/// </summary>
	class ArrowPrimitive :public BasePrimitive {
	public:
		/// <summary>
		/// データ取得
		/// </summary>
		ShapeParameter::ShapeArrow& Data() { return arrow; };
		/// <summary>
		/// メッシュ初期化
		/// </summary>
		void MeshInitialize() override { arrow.Create(mesh.get()); };
	private:
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UniqueUpdate() override;
	private:
		ShapeParameter::ShapeArrow arrow;
		ShapeParameter::ShapeArrow preArrow;
	};

	// 筒
/// <summary>
/// TubePrimitiveを管理・実装するクラス。
/// </summary>
	class TubePrimitive :public BasePrimitive {
	public:
		/// <summary>
		/// データ取得
		/// </summary>
		ShapeParameter::Tube& Data() { return tube; };
		/// <summary>
		/// メッシュ初期化
		/// </summary>
		void MeshInitialize() override { tube.Create(mesh.get()); };
	private:
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UniqueUpdate() override;
	private:
		ShapeParameter::Tube tube;
		ShapeParameter::Tube preTube;
	};

	// 角錐
/// <summary>
/// PyramidPrimitiveを管理・実装するクラス。
/// </summary>
	class PyramidPrimitive :public BasePrimitive {
	public:
		/// <summary>
		/// データ取得
		/// </summary>
		ShapeParameter::Pyramid& Data() { return pyramid; };
		/// <summary>
		/// メッシュ初期化
		/// </summary>
		void MeshInitialize() override { pyramid.Create(mesh.get()); };
	private:
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UniqueUpdate() override;
	private:
		ShapeParameter::Pyramid pyramid;
		ShapeParameter::Pyramid prePyramid;
	};

	// トーラス
/// <summary>
/// TorusPrimitiveを管理・実装するクラス。
/// </summary>
	class TorusPrimitive :public BasePrimitive {
	public:
		/// <summary>
		/// データ取得
		/// </summary>
		ShapeParameter::Torus& Data() { return torus; };
		/// <summary>
		/// メッシュ初期化
		/// </summary>
		void MeshInitialize() override { torus.Create(mesh.get()); };
	private:
		/// <summary>
		/// 固有の更新
		/// </summary>
		void UniqueUpdate() override;
	private:
		ShapeParameter::Torus torus;
		ShapeParameter::Torus preTorus;
	};
}
