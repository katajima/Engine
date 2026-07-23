#pragma once
#include "DirectXGame/engine/struct/Vector3.h"

//線分
struct Segment
{

	Vector3 origin; //!<始点
	Vector3 end;


	/// <summary>
	/// コンストラクタ
	/// </summary>
	Segment(const Vector3& o, const Vector3& e) : origin(o), end(e) {}

	/// <summary>
	/// 線分のベクトル
	/// </summary>
	Vector3 diff() const {
		return end - origin;
	}

	/// <summary>
	/// 線分の長さ
	/// </summary>
	float length() const {
		return diff().Length();
	}

	/// <summary>
	/// 単位方向ベクトル
	/// </summary>
	Vector3 normalizedDirection() const {
		return diff().Normalize();
	}

	/// <summary>
	/// 中心点
	/// </summary>
	Vector3 MidPoint() const {
		return (origin + end) * 0.5f;
	}

	/// <summary>
	/// 反転
	/// </summary>
	void Reverse() {
		std::swap(origin, end);
	}

	/// <summary>
	/// 指定された t (0.0 ~ 1.0) の位置の点を取得
	/// </summary>
	Vector3 pointAt(float t) const {
		return origin + diff() * t;
	}

	/// <summary>
	/// 最近接点
	/// </summary>
	Vector3 ClosestPoint(const Vector3& point) const {
		Vector3 ab = end - origin;
		float t = (point - origin).Dot(ab) / ab.LengthSq();
		t = std::clamp(t, 0.0f, 1.0f); // 線分上にクランプ
		return origin + ab * t;
	}

	/// <summary>
	/// 線分と点の最近接距離
	/// </summary>
	float DistanceToPoint(const Vector3& point) const {
		return (ClosestPoint(point) - point).Length();
	}

	/// <summary>
	/// 線分との最近接距離（最近接点）
	/// </summary>
	float DistanceToSegment(const Segment& other) const {
		Vector3 u = diff();
		Vector3 v = other.diff();
		Vector3 w0 = origin - other.origin;

		float a = u.Dot(u);
		float b = u.Dot(v);
		float c = v.Dot(v);
		float d = u.Dot(w0);
		float e = v.Dot(w0);

		float denom = a * c - b * b;
		float s = 0.0f, t = 0.0f;

		if (denom != 0.0f) {
			s = (b * e - c * d) / denom;
			s = std::clamp(s, 0.0f, 1.0f);
		}
		t = (a * e - b * d) / denom;
		t = std::clamp(t, 0.0f, 1.0f);

		Vector3 cp1 = pointAt(s);
		Vector3 cp2 = other.pointAt(t);
		return (cp1 - cp2).Length();
	}

	/// <summary>
	/// 線分と平面の交差
	/// </summary>
	bool IntersectPlane(const Vector3& normal, float distance, float& outT) const {
		Vector3 dir = diff();
		float denom = Dot(normal, dir);
		if (std::abs(denom) < 1e-6f) return false; // 平行

		float t = (distance - Dot(normal, origin)) / denom;
		if (t < 0.0f || t > 1.0f) return false; // 線分内でない
		outT = t;
		return true;
	}

	/// <summary>
	/// 軌跡上で最も近い点とその補間tを両方返す関数
	/// </summary>
	std::pair<Vector3, float> ClosestPointAndT(const Vector3& point) const {
		Vector3 ab = end - origin;
		float t = (point - origin).Dot(ab) / ab.LengthSq();
		t = std::clamp(t, 0.0f, 1.0f);
		return { origin + ab * t, t };
	}

	/// <summary>
	/// 線分の最近接端点（始点・終点のどちらが近いか）
	/// </summary>
	Vector3 NearestEndPoint(const Vector3& point) const {
		float d0 = (point - origin).LengthSq();
		float d1 = (point - end).LengthSq();
		return (d0 < d1) ? origin : end;
	}

	/// <summary>
	/// 長さが一定未満かどうかチェック
	/// </summary>
	bool IsShorterThan(float threshold) const {
		return diff().LengthSq() < threshold * threshold;
	}
};
// 直線
struct Line
{
	Vector3 origin; //!<始点
	Vector3 diff;   //!<終点への差分ベクトル

	/// <summary>
	/// 指定位置（t）における点取得
	/// </summary>
	Vector3 PointAt(float t) const {
		return origin + diff * t;
	}

	/// <summary>
	/// 正規化された方向ベクトルの取得
	/// </summary>
	Vector3 Direction() const {
		return Normalize(diff);
	}

	/// <summary>
	/// 任意の点との最近接点
	/// </summary>
	Vector3 ClosestPoint(const Vector3& point) const {
		Vector3 dir = Direction();
		float t = Dot(point - origin, dir);
		return origin + dir * t;
	}

	/// <summary>
	/// 点との距離
	/// </summary>
	float DistanceToPoint(const Vector3& point) const {
		return (ClosestPoint(point) - point).Length();
	}

	/// <summary>
	/// Lineの可視化セグメント生成
	/// </summary>
	Segment ToDebugSegment(float tMin = -50.0f, float tMax = +50.0f) const {
		Vector3 dir = Normalize(diff);
		return Segment(origin + dir * tMin, origin + dir * tMax);
	}

	/// <summary>
	/// 2直線の最短距離（空間直線 vs 空間直線）
	/// </summary>
	float DistanceToLine(const Line& other) const {
		Vector3 u = diff;
		Vector3 v = other.diff;
		Vector3 w0 = origin - other.origin;

		float a = Dot(u, u);
		float b = Dot(u, v);
		float c = Dot(v, v);
		float d = Dot(u, w0);
		float e = Dot(v, w0);

		float denom = a * c - b * b;
		float s, t;
		if (denom != 0.0f) {
			s = (b * e - c * d) / denom;
			t = (a * e - b * d) / denom;
		}
		else {
			s = 0.0f;
			t = e / c;
		}

		Vector3 p1 = origin + u * s;
		Vector3 p2 = other.origin + v * t;
		return (p1 - p2).Length();
	}
};
//半直線
struct Ray
{
	Vector3 origin; //!<始点
	Vector3 diff;   //!<終点への差分ベクトル

	/// <summary>
	/// Ray上の点取得
	/// </summary>
	Vector3 PointAt(float t) const {
		return origin + diff * t;
	}

	/// <summary>
	/// 方向の取得
	/// </summary>
	Vector3 Direction() const {
		return Normalize(diff);
	}

	/// <summary>
	/// Rayと点の最近接点（t>=0に制限）
	/// </summary>
	Vector3 ClosestPoint(const Vector3& point) const {
		Vector3 dir = Direction();
		float t = Dot(point - origin, dir);
		t = (std::max)(t, 0.0f); // 半直線なので t < 0 は使わない
		return origin + dir * t;
	}

	/// <summary>
	/// Rayの可視化セグメント生成
	/// </summary>
	Segment ToDebugSegment(float length = 100.0f) const {
		return Segment(origin, origin + Direction() * length);
	}

	/// <summary>
	/// 鏡面反射ベクトル
	/// </summary>
	Vector3 Reflect(const Vector3& normal) const {
		return diff - (normal * Dot(diff, normal)) * 2.0f;
	}

	/// <summary>
	/// 球との交差（中心と半径を渡す）
	/// </summary>
	bool IntersectSphere(const Vector3& center, float radius, float& outT) const {
		Vector3 dir = Direction();
		Vector3 oc = origin - center;
		float b = Dot(oc, dir);
		float c = Dot(oc, oc) - radius * radius;
		float h = b * b - c;
		if (h < 0.0f) return false;
		h = sqrt(h);
		outT = -b - h;
		if (outT < 0.0f) outT = -b + h;
		return outT >= 0.0f;
	}
};

