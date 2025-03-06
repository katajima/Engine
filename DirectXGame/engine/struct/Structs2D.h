#pragma once
#include "DirectXGame/engine/struct/Vector2.h"
#include "DirectXGame/engine/struct/Matrix3x3.h"

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

	bool intersects(const Box& other) const {
		return (min_.x <= other.max_.x && max_.x >= other.min_.x &&
			min_.y <= other.max_.y && max_.y >= other.min_.y &&);
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

	Triangle2D OffsetVector2(const Vector2& offset) const {
		Triangle2D result = *this;  // コピーを作成
		for (auto& vertex : result.vertices) {
			vertex += offset;
		}
		return result;
	}



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

struct Ball2D {
	Vector2 position;		//位置
	Vector2 veloctiy;		//速度
	Vector2 acceleration;	//加速度
	float mass;				//質量
	float radius;			//半径
	unsigned int color;		//色
};

struct Pendulum2D {
	Vector2 anchor;				// アンカーポイント
	float length;				// 紐の長さ
	float angle;				// 現在の角度
	float angularVelocity;		// 角度ω
	float angularAcceleration;	// 角加速度
};

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

struct OBB2D {
	Vector2 center;
	Vector2 orientations[3];
	Vector2 size;
};