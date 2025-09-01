#pragma once
#include "DirectXGame/engine/math/MathFanctions.h"
#include "DirectXGame/engine/struct/Structs3D.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"


// ------------------------------------
// 形状基底クラス
// ------------------------------------

class EmitterShape
{
public:

	void Init() { worldTransform_.Initialize(); }
	WorldTransform worldTransform_;
};

// ------------------------------------
// 形状派生クラス
// ------------------------------------


// 点
class EmitterPoint : public EmitterShape
{
public:
};
// 線
class EmitterLine : public EmitterShape
{
public:
	Segment line = Segment(Vector3{ 0,0,0 }, Vector3{ 1,0,0 });
};
// 円
class EmitterCorner : public EmitterShape
{
public:
	CornerSegment corner;
};
// スプライン
class EmitterSpline : public EmitterShape
{
public:
	std::vector<Vector3> controlPoints; // 各ポジション
};
// 球
//class EmitterSphere : public EmitterShape
//{
//public:
//	float radius = 1.0f;
//};
// 箱
class EmitterBox : public EmitterShape
{
public:
	Vector3 size = Vector3{ 1,1,1 };
};


