#pragma once
#include "DirectXGame/engine/struct/Vector2.h"
#include "DirectXGame/engine/struct/Matrix3x3.h"
#include "array"


//Transform
struct Transform2D {
	Vector2 scale;
	float rotate;
	Vector2 translate;
};

//Box
struct Box {
	Vector2 min_; //!< 最小点
	Vector2 max_; //!< 最大点

	Box(Vector2 min = Vector2(), Vector2 max = Vector2()) : min_(min), max_(max) {}

	// 判定
	bool intersects(const Box& other) const {
		return (min_.x <= other.max_.x && max_.x >= other.min_.x &&
			min_.y <= other.max_.y && max_.y >= other.min_.y);
	}
	// 判定
	bool intersects(const Vector2& other) const {
		return (min_.x <= other.x && max_.x >= other.x &&
			min_.y <= other.y && max_.y >= other.y);
	}

};



// OBB
struct BoxOBB {
	Vector2 center_;		//!< 中心座標
	Vector2 halfSize_;		//!< 幅/2, 高さ/2
	float rotation_;		//!< ラジアンでの回転角（時計回り）
	Vector2 corners_[4];	// コーナー情報
	BoxOBB(Vector2 center = {}, Vector2 halfSize = {}, float rotation = 0.0f)
		: center_(center), halfSize_(halfSize), rotation_(rotation) {
	}

	// 角取得
	std::array<Vector2, 4> GetCorners() const {
		// 回転行列
		float cosA = std::cos(rotation_);
		float sinA = std::sin(rotation_);

		Vector2 localCorners[4] = {
			{-halfSize_.x, -halfSize_.y},  // 左下
			{ halfSize_.x, -halfSize_.y},  // 右下
			{ halfSize_.x,  halfSize_.y},  // 右上
			{-halfSize_.x,  halfSize_.y},  // 左上
		};

		std::array<Vector2, 4> worldCorners;
		for (int i = 0; i < 4; ++i) {
			float x = localCorners[i].x * cosA - localCorners[i].y * sinA;
			float y = localCorners[i].x * sinA + localCorners[i].y * cosA;
			worldCorners[i] = { center_.x + x, center_.y + y };
		}
		return worldCorners;
	}

	// 判定
	bool intersects(const Vector2& point) const {
		auto corners = GetCorners();

		// 三角形2つに分割して点が含まれるか確認
		return PointInQuad(point, corners);
	}


	// 判定
	bool intersects(const BoxOBB& other) const {
		auto aCorners = this->GetCorners();
		auto bCorners = other.GetCorners();

		// 判定軸（各辺の法線）を取得（4本）
		std::array<Vector2, 4> axes = {
			GetEdgeNormal(aCorners[0], aCorners[1]),
			GetEdgeNormal(aCorners[1], aCorners[2]),
			GetEdgeNormal(bCorners[0], bCorners[1]),
			GetEdgeNormal(bCorners[1], bCorners[2]),
		};

		for (const auto& axis : axes) {
			// 各矩形を軸に投影してオーバーラップ確認
			auto [minA, maxA] = ProjectOntoAxis(aCorners, axis);
			auto [minB, maxB] = ProjectOntoAxis(bCorners, axis);

			if (maxA < minB || maxB < minA) {
				return false; // 分離軸あり → 交差していない
			}
		}

		return true; // 全軸で重なっている → 交差
	}


// 点が内側にあるか
	bool PointInQuad(const Vector2& p, const std::array<Vector2, 4>& quad) const {
		// 三角形 1: [0, 1, 2], 三角形 2: [2, 3, 0]
		return PointInTriangle(p, quad[0], quad[1], quad[2]) ||
			PointInTriangle(p, quad[2], quad[3], quad[0]);
	}
	// 点が三角形内にあるか
	bool PointInTriangle(const Vector2& p, const Vector2& a, const Vector2& b, const Vector2& c) const {
		auto sign = [](const Vector2& p1, const Vector2& p2, const Vector2& p3) {
			return (p1.x - p3.x) * (p2.y - p3.y) -
				(p2.x - p3.x) * (p1.y - p3.y);
			};

		bool b1 = sign(p, a, b) < 0.0f;
		bool b2 = sign(p, b, c) < 0.0f;
		bool b3 = sign(p, c, a) < 0.0f;

		return (b1 == b2) && (b2 == b3);
	}

	// 辺の法線方向
	Vector2 GetEdgeNormal(const Vector2& p1, const Vector2& p2) const {
		Vector2 edge = { p2.x - p1.x, p2.y - p1.y };
		return { -edge.y, edge.x }; // 法線ベクトル（右手系）
	}

	// 軸への射影
	std::pair<float, float> ProjectOntoAxis(const std::array<Vector2, 4>& points, const Vector2& axis)const {
		float min = Dot(points[0], axis);
		float max = min;

		for (int i = 1; i < 4; ++i) {
			float projection = Dot(points[i], axis);
			min = (std::min)(min, projection);
			max = (std::max)(max, projection);
		}
		return { min, max };
	}
};

// 球
struct Sphere2D {
	Vector2 center; //!<中心点
	float radius;   //!<半径 
};


// 直線
struct Line2D
{
	Vector2 origin; //!<始点
	Vector2 diff;   //!<終点への差分ベクトル
};
//半直線
struct Ray2D
{
	Vector2 origin; //!<始点
	Vector2 diff;   //!<終点への差分ベクトル
};

//線分
struct Segment2D
{

	Vector2 origin; //!<始点
	Vector2 end;


	// コンストラクタ
	Segment2D(const Vector2& o, const Vector2& e) : origin(o), end(e) {}

	// 線分のベクトル
	Vector2 diff() const {
		return end - origin;
	}

	// 線分の長さ
	float length() const {
		return diff().Length();
	}

	// 単位方向ベクトル
	Vector2 normalizedDirection() const {
		return diff().Normalize();
	}

	// 指定された t (0.0 ~ 1.0) の位置の点を取得
	Vector2 pointAt(float t) const {
		return origin + diff() * t;
	}
};

struct CornerSegment2D {
	Vector2 center;
	int segment;
	float radius;
};

//三角形
struct Triangle2D
{
	Vector2 vertices[3]; // !頂点
	Box bounds;

	// +=オペレーターのオーバーロード 
	Triangle2D& operator+=(const Vector2& offset) {
		for (auto& vertex : vertices) {
			vertex += offset;
		}
		return *this;
	}

	// オフセット
	Triangle2D OffsetVector2(const Vector2& offset) const {
		Triangle2D result = *this;  // コピーを作成
		for (auto& vertex : result.vertices) {
			vertex += offset;
		}
		return result;
	}


	// コンストラクタ
	Triangle2D(Vector2 v0, Vector2 v1, Vector2 v2) : vertices{ v0, v1, v2 } {
		bounds.min_ = Min(Min(v0, v1), v2);
		bounds.max_ = Max(Max(v0, v1), v2);
	}
};

//ばね
struct Spring2D
{
	Vector2 anchor;      // アンカーの固定された端の位置
	float naturalLength; // 自然長
	float stiffness;     // 剛性。バネ定数k
	float dampingCoefficient; // 減衰係数
};

// ボール
struct Ball2D {
	Vector2 position;		//位置
	Vector2 veloctiy;		//速度
	Vector2 acceleration;	//加速度
	float mass;				//質量
	float radius;			//半径
	unsigned int color;		//色
};

// 振り子
struct Pendulum2D {
	Vector2 anchor;				// アンカーポイント
	float length;				// 紐の長さ
	float angle;				// 現在の角度
	float angularVelocity;		// 角度ω
	float angularAcceleration;	// 角加速度
};
// 円錐振り子
struct ConicalPendulum2D {
	Vector2 anchor;				// アンカーポイント
	float length;				// 紐の長さ
	float halfApexAngle;		// 円錐の頂角の半分
	float angle;				// 現在の角度
	float angularVelocity;		// 角度ω

};

// カプセル
struct Capsule2D
{
	Segment2D segment;
	float radius;


	// コンストラクタ
	Capsule2D(const Vector2& p0, const Vector2& p1, float r) : segment(p0, p1), radius(r) {}

	// カプセルの AABB を取得
	Box computeAABB() const {
		Vector2 minPoint = Min(segment.origin, segment.end) - Vector2(radius, radius);
		Vector2 maxPoint = Max(segment.origin, segment.end) + Vector2(radius, radius);
		return Box(minPoint, maxPoint);
	}
};

// OBB
struct OBB2D {
	Vector2 center;
	Vector2 halfSize;    // 半サイズ（軸方向）
	Vector2 axisX;       // 単位ベクトル：ローカルX軸
	Vector2 axisZ;       // 単位ベクトル：ローカルZ軸
};