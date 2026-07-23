#pragma once

#include"DirectXGame/engine/struct/Structs3D.h"
#include "DirectXGame/engine/Mesh/ModelMesh.h"
#include"DirectXGame/engine/math/MathFunctions.h"

namespace Engine {
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
			/// <summary>
			/// 生成
			/// </summary>
			void Create(ModelMesh* mesh, bool isInv = false);

			/// <summary>
			/// == オペレーター
			/// </summary>
			bool operator==(const ShapePlane& other) const {
				return width == other.width && height == other.height;
			}

			/// <summary>
			/// != オペレーター
			/// </summary>
			bool operator!=(const ShapePlane& other) const {
				return !(*this == other);
			}
		};

		// 三角
		struct ShapeTriangle {
			Vector2 upPos = { 0.0f,1.0f };
			Vector2 leftPos = { 1.0f,-1.0f };
			Vector2 rightPos = { -1.0f,-1.0f };
			/// <summary>
			/// 生成
			/// </summary>
			void Create(ModelMesh* mesh);

			/// <summary>
			/// == オペレーター
			/// </summary>
			bool operator==(const ShapeTriangle& other) const {
				return upPos == other.upPos && leftPos == other.leftPos && rightPos == other.rightPos;
			}

			/// <summary>
			/// != オペレーター
			/// </summary>
			bool operator!=(const ShapeTriangle& other) const {
				return !(*this == other);
			}

		};
		// 十字
		struct ShapeCross {
			float armLength = 10;
			float armWidth = 5;
			/// <summary>
			/// 生成
			/// </summary>
			void Create(ModelMesh* mesh);

			/// <summary>
			/// == オペレーター
			/// </summary>
			bool operator==(const ShapeCross& other) const {
				return armLength == other.armLength && armWidth == other.armWidth;
			}

			/// <summary>
			/// != オペレーター
			/// </summary>
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

			/// <summary>
			/// == オペレーター
			/// </summary>
			bool operator==(const AnimationPlane& other) const {
				return flag == other.flag && num == other.num && count == other.count && interval == other.interval && width == other.width && height == other.height && direction == other.direction;
			}

			/// <summary>
			/// != オペレーター
			/// </summary>
			bool operator!=(const AnimationPlane& other) const {
				return !(*this == other);
			}
		};

		// 四角
		struct ShapeCube
		{
			Vector3 size = { 1,1,1 };
			/// <summary>
			/// 生成
			/// </summary>
			void Create(ModelMesh* mesh);

			/// <summary>
			/// == オペレーター
			/// </summary>
			bool operator==(const ShapeCube& other) const {
				return size == other.size;
			}

			/// <summary>
			/// != オペレーター
			/// </summary>
			bool operator!=(const ShapeCube& other) const {
				return !(*this == other);
			}

		};

		// 円
		struct Circle
		{

			float radius = 5.0f;
			int segments = 16;
			/// <summary>
			/// 生成
			/// </summary>
			void Create(ModelMesh* mesh);


			/// <summary>
			/// == オペレーター
			/// </summary>
			bool operator==(const Circle& other) const {
				return radius == other.radius && segments == other.segments;
			}

			/// <summary>
			/// != オペレーター
			/// </summary>
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

			/// <summary>
			/// 生成
			/// </summary>
			void Create(ModelMesh* mesh);

			/// <summary>
			/// == オペレーター
			/// </summary>
			bool operator==(const Star& other) const {
				return innerRadius == other.innerRadius && outerRadius == other.outerRadius && segments == other.segments;
			}

			/// <summary>
			/// != オペレーター
			/// </summary>
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
			/// <summary>
			/// 生成
			/// </summary>
			void Create(ModelMesh* mesh);

			/// <summary>
			/// == オペレーター
			/// </summary>
			bool operator==(const Crescent& other) const {
				return innerRadius == other.innerRadius && outerRadius == other.outerRadius && segments == other.segments && distance == other.distance;
			}

			/// <summary>
			/// != オペレーター
			/// </summary>
			bool operator!=(const Crescent& other) const {
				return !(*this == other);
			}
		};
		// リング
		struct Ring {
			float innerRadius = 2.0f;
			float outerRadius = 5.0f;
			int segments = 16;
			/// <summary>
			/// 生成
			/// </summary>
			void Create(ModelMesh* mesh);

			/// <summary>
			/// == オペレーター
			/// </summary>
			bool operator==(const Ring& other) const {
				return innerRadius == other.innerRadius && outerRadius == other.outerRadius && segments == other.segments;
			}

			/// <summary>
			/// != オペレーター
			/// </summary>
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
			/// <summary>
			/// 生成
			/// </summary>
			void Create(ModelMesh* mesh);

			/// <summary>
			/// == オペレーター
			/// </summary>
			bool operator==(const ShapeSphere& other) const {
				return radius == other.radius && latitudeSegments == other.latitudeSegments && longitudeSegments == other.longitudeSegments && isTopBased == other.isTopBased;
			}

			/// <summary>
			/// != オペレーター
			/// </summary>
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
			/// <summary>
			/// 生成
			/// </summary>
			void Create(ModelMesh* mesh);

			/// <summary>
			/// == オペレーター
			/// </summary>
			bool operator==(const ShapeArrow& other) const {
				return shaftLength == other.shaftLength && shaftRadius == other.shaftRadius && headLength == other.headLength && headRadius == other.headRadius && segments == other.segments;
			}

			/// <summary>
			/// != オペレーター
			/// </summary>
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
			/// <summary>
			/// 生成
			/// </summary>
			void Create(ModelMesh* mesh);

			/// <summary>
			/// == オペレーター
			/// </summary>
			bool operator==(const Cylinder& other) const {
				return innerRadius == other.innerRadius && outerRadius == other.outerRadius && height == other.height && segments == other.segments && isCover == other.isCover;
			}

			/// <summary>
			/// != オペレーター
			/// </summary>
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
			/// <summary>
			/// 生成
			/// </summary>
			void Create(ModelMesh* mesh);


			/// <summary>
			/// == オペレーター
			/// </summary>
			bool operator==(const Tube& other) const {
				return innerRadius == other.innerRadius && radius == other.radius && segments == other.segments && height == other.height;
			}

			/// <summary>
			/// != オペレーター
			/// </summary>
			bool operator!=(const Tube& other) const {
				return !(*this == other);
			}
		};
		// 角錐
		struct Pyramid {
			float radius = 3.0f;
			float height = 3.0f;
			int segments = 16;
			/// <summary>
			/// 生成
			/// </summary>
			void Create(ModelMesh* mesh);

			/// <summary>
			/// == オペレーター
			/// </summary>
			bool operator==(const Pyramid& other) const {
				return radius == other.radius && segments == other.segments && height == other.height;
			}

			/// <summary>
			/// != オペレーター
			/// </summary>
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
			/// <summary>
			/// 生成
			/// </summary>
			void Create(ModelMesh* mesh);

			/// <summary>
			/// == オペレーター
			/// </summary>
			bool operator==(const Torus& other) const {
				return innerRadius == other.innerRadius && segments == other.segments && outerRadius == other.outerRadius && tubeSegments == other.tubeSegments;
			}

			/// <summary>
			/// != オペレーター
			/// </summary>
			bool operator!=(const Torus& other) const {
				return !(*this == other);
			}
		};
	}
}