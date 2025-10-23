#pragma once

#include"DirectXGame/engine/struct/Structs3D.h"
#include "DirectXGame/engine/Mesh/ModelMesh.h"
#include"DirectXGame/engine/math/MathFanctions.h"


// 各シェイプのパラメーター
namespace ShapeParameter {
	// シェイプタイプ
	enum class ShapeType {
		None = -1,
		Plane,
		Triangle,
		Cross,
		Cube,
		Circle,
		Star,
		Crescent,
		Ring,
		Sphere,
		Arrow,
		Cylinder,
		Tube,
		Pyramid,
		Torus,
		Max
	};

	// 平面
	struct ShapePlane {
		float width = 1.0f;
		float height = 1.0f;
		// 生成
		void Create(ModelMesh* mesh, bool isInv = false);

		// == オペレーター
		bool operator==(const ShapePlane& other) const {
			return width == other.width && height == other.height;
		}

		// != オペレーター
		bool operator!=(const ShapePlane& other) const {
			return !(*this == other);
		}
	};

	// 三角
	struct ShapeTriangle {
		Vector2 upPos = { 0.0f,1.0f };
		Vector2 leftPos = { 1.0f,-1.0f };
		Vector2 rightPos = { -1.0f,-1.0f };
		// 生成
		void Create(ModelMesh* mesh);

		// == オペレーター
		bool operator==(const ShapeTriangle& other) const {
			return upPos == other.upPos && leftPos == other.leftPos && rightPos == other.rightPos;
		}

		// != オペレーター
		bool operator!=(const ShapeTriangle& other) const {
			return !(*this == other);
		}

	};
	// 十字
	struct ShapeCross {
		float armLength = 10;
		float armWidth = 5;
		// 生成
		void Create(ModelMesh* mesh);

		// == オペレーター
		bool operator==(const ShapeCross& other) const {
			return armLength == other.armLength && armWidth == other.armWidth;
		}

		// != オペレーター
		bool operator!=(const ShapeCross& other) const {
			return !(*this == other);
		}
	};

	// ばね
	struct Spring
	{
		float length = 1;
		float width = 1;
		float height = 1;
		int segments = 3;
		float spacing = 1;
		int turns = 3;
		float thickness = 2;
	};

	// アニメーション平面
	struct AnimationPlane {
		bool flag;
		bool isLoop;
		bool isUV;
		int num;
		int count;
		float interval;
		float width;
		float height;
		Vector2 direction;

		// == オペレーター
		bool operator==(const AnimationPlane& other) const {
			return flag == other.flag && num == other.num && count == other.count && interval == other.interval && width == other.width && height == other.height && direction == other.direction;
		}

		// != オペレーター
		bool operator!=(const AnimationPlane& other) const {
			return !(*this == other);
		}
	};

	// 四角
	struct ShapeCube
	{
		Vector3 size = { 1,1,1 };
		// 生成
		void Create(ModelMesh* mesh);

		// == オペレーター
		bool operator==(const ShapeCube& other) const {
			return size == other.size;
		}

		// != オペレーター
		bool operator!=(const ShapeCube& other) const {
			return !(*this == other);
		}

	};

	// 円
	struct Circle
	{

		float radius = 5.0f;
		int segments = 16;
		// 生成
		void Create(ModelMesh* mesh);


		// == オペレーター
		bool operator==(const Circle& other) const {
			return radius == other.radius && segments == other.segments;
		}

		// != オペレーター
		bool operator!=(const Circle& other) const {
			return !(*this == other);
		}
	};

	// 星
	struct Star
	{
		float innerRadius = 2.0f;
		float outerRadius = 5.0f;
		int segments = 5;

		// 生成
		void Create(ModelMesh* mesh);

		// == オペレーター
		bool operator==(const Star& other) const {
			return innerRadius == other.innerRadius && outerRadius == other.outerRadius && segments == other.segments;
		}

		// != オペレーター
		bool operator!=(const Star& other) const {
			return !(*this == other);
		}
	};
	// 三日月
	struct Crescent
	{
		float innerRadius = 2.0f;
		float outerRadius = 5.0f;
		int segments = 16;
		float distance = 5.0f;
		// 生成
		void Create(ModelMesh* mesh);

		// == オペレーター
		bool operator==(const Crescent& other) const {
			return innerRadius == other.innerRadius && outerRadius == other.outerRadius && segments == other.segments && distance == other.distance;
		}

		// != オペレーター
		bool operator!=(const Crescent& other) const {
			return !(*this == other);
		}
	};
	// リング
	struct Ring {
		float innerRadius = 2.0f;
		float outerRadius = 5.0f;
		int segments = 16;
		// 生成
		void Create(ModelMesh* mesh);

		// == オペレーター
		bool operator==(const Ring& other) const {
			return innerRadius == other.innerRadius && outerRadius == other.outerRadius && segments == other.segments;
		}

		// != オペレーター
		bool operator!=(const Ring& other) const {
			return !(*this == other);
		}
	};
	// 球
	struct ShapeSphere {
		float radius = 5.0f;
		int latitudeSegments = 16;
		int longitudeSegments = 16;
		bool isTopBased = false;
		// 生成
		void Create(ModelMesh* mesh);

		// == オペレーター
		bool operator==(const ShapeSphere& other) const {
			return radius == other.radius && latitudeSegments == other.latitudeSegments && longitudeSegments == other.longitudeSegments && isTopBased == other.isTopBased;
		}

		// != オペレーター
		bool operator!=(const ShapeSphere& other) const {
			return !(*this == other);
		}
	};
	// 矢印
	struct ShapeArrow {
		float shaftLength = 16.0f;
		float shaftRadius = 4.0f;
		float headLength = 8.0f;
		float headRadius = 8.0f;
		int segments = 16;
		// 生成
		void Create(ModelMesh* mesh);

		// == オペレーター
		bool operator==(const ShapeArrow& other) const {
			return shaftLength == other.shaftLength && shaftRadius == other.shaftRadius && headLength == other.headLength && headRadius == other.headRadius && segments == other.segments;
		}

		// != オペレーター
		bool operator!=(const ShapeArrow& other) const {
			return !(*this == other);
		}

	};
	// 円柱
	struct Cylinder {
		float innerRadius = 3.0f;
		float outerRadius = 3.0f;
		float height = 10.0f;
		int segments = 16;
		bool isCover = true;
		// 生成
		void Create(ModelMesh* mesh);

		// == オペレーター
		bool operator==(const Cylinder& other) const {
			return innerRadius == other.innerRadius && outerRadius == other.outerRadius && height == other.height && segments == other.segments && isCover == other.isCover;
		}

		// != オペレーター
		bool operator!=(const Cylinder& other) const {
			return !(*this == other);
		}
	};
	// 筒
	struct Tube {
		float radius = 3.0f;
		float innerRadius = 2.0f;
		float height = 10.0f;
		int segments = 16;
		// 生成
		void Create(ModelMesh* mesh);


		// == オペレーター
		bool operator==(const Tube& other) const {
			return innerRadius == other.innerRadius && radius == other.radius && segments == other.segments && height == other.height;
		}

		// != オペレーター
		bool operator!=(const Tube& other) const {
			return !(*this == other);
		}
	};
	// 角錐
	struct Pyramid {
		float radius = 3.0f;
		float height = 3.0f;
		int segments = 16;
		// 生成
		void Create(ModelMesh* mesh);

		// == オペレーター
		bool operator==(const Pyramid& other) const {
			return radius == other.radius && segments == other.segments && height == other.height;
		}

		// != オペレーター
		bool operator!=(const Pyramid& other) const {
			return !(*this == other);
		}
	};
	// トーラス
	struct Torus {
		float innerRadius = 3.0f;
		float outerRadius = 3.0f;
		int segments = 16;
		int tubeSegments = 16;
		// 生成
		void Create(ModelMesh* mesh);

		// == オペレーター
		bool operator==(const Torus& other) const {
			return innerRadius == other.innerRadius && segments == other.segments && outerRadius == other.outerRadius && tubeSegments == other.tubeSegments;
		}

		// != オペレーター
		bool operator!=(const Torus& other) const {
			return !(*this == other);
		}
	};




}
