#pragma once
#include "DirectXGame/engine/struct/Vector4.h"
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/struct/Vector2.h"
#include "DirectXGame/engine/struct/Quaternion.h"
#include "DirectXGame/engine/struct/Matrix4x4.h"
#include "DirectXGame/engine/struct/Matrix3x3.h"




//Transform
struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

//AABB
struct AABB {
	Vector3 min_; //!< 最小点
	Vector3 max_; //!< 最大点

	AABB(Vector3 min = Vector3(), Vector3 max = Vector3()) : min_(min), max_(max) {}

	bool intersects(const AABB& other) const {
		return (min_.x <= other.max_.x && max_.x >= other.min_.x &&
			min_.y <= other.max_.y && max_.y >= other.min_.y &&
			min_.z <= other.max_.z && max_.z >= other.min_.z);
	}
};


// 球
struct Sphere {
	Vector3 center; //!<中心点
	float radius;   //!<半径 
};

//平面
struct Plane {
	Vector3 normal;  //!< 法線
	float distance; //!< 距離 
};

// 直線
struct Line
{
	Vector3 origin; //!<始点
	Vector3 diff;   //!<終点への差分ベクトル
};
//半直線
struct Ray
{
	Vector3 origin; //!<始点
	Vector3 diff;   //!<終点への差分ベクトル
};

//線分
struct Segment
{
	
	Vector3 origin; //!<始点
	Vector3 end;
	

	// コンストラクタ
	Segment(const Vector3& o, const Vector3& e) : origin(o), end(e) {}

	// 線分のベクトル
	Vector3 diff() const {
		return end - origin;
	}

	// 線分の長さ
	float length() const {
		return diff().Length();
	}

	// 単位方向ベクトル
	Vector3 normalizedDirection() const {
		return diff().Normalize();
	}

	// 指定された t (0.0 ~ 1.0) の位置の点を取得
	Vector3 pointAt(float t) const {
		return origin + diff() * t;
	}
};

struct CornerSegment {
	Vector3 center;
	int segment;
	float radius;
};


//三角形
struct Triangle
{
	Vector3 vertices[3]; // !頂点
	AABB bounds;

	// +=オペレーターのオーバーロード 
	Triangle& operator+=(const Vector3& offset) { 
		for (auto& vertex : vertices) { 
			vertex += offset; 
		} 
		return *this; 
	}

	Triangle OffsetVector3(const Vector3& offset) const {
		Triangle result = *this;  // コピーを作成
		for (auto& vertex : result.vertices) {
			vertex += offset;
		}
		return result;
	}

	

	Triangle(Vector3 v0, Vector3 v1, Vector3 v2) : vertices{ v0, v1, v2 } {
		bounds.min_ = Min(Min(v0, v1), v2);
		bounds.max_ = Max(Max(v0, v1), v2);
	}
};

//ばね
struct Spring
{
	Vector3 anchor;      // アンカーの固定された端の位置
	float naturalLength; // 自然長
	float stiffness;     // 剛性。バネ定数k
	float dampingCoefficient; // 減衰係数
};

struct Ball {
	Vector3 position;		//位置
	Vector3 veloctiy;		//速度
	Vector3 acceleration;	//加速度
	float mass;				//質量
	float radius;			//半径
	unsigned int color;		//色
};

struct Pendulum {
	Vector3 anchor;				// アンカーポイント
	float length;				// 紐の長さ
	float angle;				// 現在の角度
	float angularVelocity;		// 角度ω
	float angularAcceleration;	// 角加速度
};

struct ConicalPendulum {
	Vector3 anchor;				// アンカーポイント
	float length;				// 紐の長さ
	float halfApexAngle;		// 円錐の頂角の半分
	float angle;				// 現在の角度
	float angularVelocity;		// 角度ω

};

// カプセル
struct Capsule
{
	Segment segment;
	float radius;


	// コンストラクタ
	Capsule(const Vector3& p0, const Vector3& p1, float r) : segment(p0, p1), radius(r) {}

	// カプセルの AABB を取得
	AABB computeAABB() const {
		Vector3 minPoint = Min(segment.origin, segment.end) - Vector3(radius, radius, radius);
		Vector3 maxPoint = Max(segment.origin, segment.end) + Vector3(radius, radius, radius);
		return AABB(minPoint, maxPoint);
	}
};

struct OBB {
	Vector3 center;
	Vector3 orientations[3];
	Vector3 size;
};
