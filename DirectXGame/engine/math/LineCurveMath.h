#pragma once
#include "DirectXGame/engine/math/MathFanctions.h"


#pragma region 線や曲線などの汎用的な計算

// ベジェ曲線
static Vector3 Bezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, float t) {

	Vector3 p0p1 = Lerp(p0, p1, t);

	Vector3 p1p2 = Lerp(p1, p2, t);

	Vector3 p = Lerp(p0p1, p1p2, t);

	return Lerp(p0p1, p1p2, t);

}

// スプライン曲線
static Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
	const float s = 0.5f; // Catmull-Rom スプラインのスケール

	float t2 = t * t;  // tの2乗
	float t3 = t2 * t; // tの3乗

	// 係数の計算
	Vector3 e3 = {
		(-p0.x + 3 * p1.x - 3 * p2.x + p3.x),
		(-p0.y + 3 * p1.y - 3 * p2.y + p3.y),
		(-p0.z + 3 * p1.z - 3 * p2.z + p3.z)
	};

	Vector3 e2 = {
		(2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x),
		(2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y),
		(2 * p0.z - 5 * p1.z + 4 * p2.z - p3.z)
	};

	Vector3 e1 = {
		(-p0.x + p2.x),
		(-p0.y + p2.y),
		(-p0.z + p2.z)
	};

	Vector3 e0 = {
		(2 * p1.x),
		(2 * p1.y),
		(2 * p1.z)
	};

	// Catmull-Rom 補間計算
	Vector3 pt = {
		(e3.x * t3 + e2.x * t2 + e1.x * t + e0.x) * s,
		(e3.y * t3 + e2.y * t2 + e1.y * t + e0.y) * s,
		(e3.z * t3 + e2.z * t2 + e1.z * t + e0.z) * s
	};

	return pt;
}

// スプライン曲線
static Vector3 CatmullRom(const std::vector<Vector3>& points, float t) {
	assert(points.size() >= 4 && "制御点は4点以上必要です");

	// 区間数（`N-1` の計算）
	size_t division = points.size() - 1;
	if (division == 0) return points[0]; // 例外ケース処理

	// 1区間の長さ（全体を1.0とした場合）
	float areaWidth = 1.0f / division;

	// `t` を `areaWidth` で割り、区間番号を取得
	size_t index = static_cast<size_t>(t / areaWidth);
	index = Clamp(index, static_cast<size_t>(0), division - 1);

	// 各点のインデックス
	size_t index0 = (index == 0) ? 0 : index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;

	// `index3` の制限 (最後の区間では p3 = p2)
	if (index3 >= points.size()) index3 = points.size() - 1;

	// 4点の座標
	const Vector3& p0 = points[index0];
	const Vector3& p1 = points[index1];
	const Vector3& p2 = points[index2];
	const Vector3& p3 = points[index3];

	// 区間内の `t` を正規化
	float t_2 = static_cast<float>(t - index * areaWidth) / areaWidth;
	t_2 = Clamp(t_2, 0.0f, 1.0f);

	// 4点を指定して Catmull-Rom 補間
	return CatmullRom(p0, p1, p2, p3, t_2);
}

// 最近接点
static Vector3 ClosestPoint(const Vector3& point, const Segment& segment)
{
	Vector3 result{};
	Vector3 d = segment.diff();
	Vector3 v = {
		point.x - segment.origin.x,
		point.y - segment.origin.y,
		point.z - segment.origin.z
	};


	float dot = Dot(d, d);
	if (dot == 0.0f) {
		return segment.origin;
	}

	float t = Dot(v, d) / dot;

	t = Clamp(t);

	result.x = segment.origin.x + d.x * t;
	result.y = segment.origin.y + d.y * t;
	result.z = segment.origin.z + d.z * t;

	return result;
}

// 点と線分間の距離の二乗を求める関数
static float PointLineDistanceSquared(const Vector3& point, const Vector3& a, const Vector3& b)
{
	Vector3 ab = b - a;
	Vector3 ap = point - a;
	float abLengthSquared = ab.LengthSq();

	if (abLengthSquared == 0.0f) {
		return ap.LengthSq(); // 線分が点の場合
	}

	float t = ap.Dot(ab) / abLengthSquared;
	t = (std::max)(0.0f, (std::min)(1.0f, t));

	Vector3 closestPoint = a + ab * t;
	return (point - closestPoint).LengthSq();
}

// 線分と線分間の距離の二乗を求める関数
static float SegmentSegmentDistanceSquared(const Segment& seg1, const Segment& seg2)
{
	Vector3 u = seg1.diff();
	Vector3 v = seg2.diff();
	Vector3 w = seg1.origin - seg2.origin;

	float a = u.Dot(u);  // |u|^2
	float b = u.Dot(v);
	float c = v.Dot(v);  // |v|^2
	float d = u.Dot(w);
	float e = v.Dot(w);

	float denom = a * c - b * b;
	float s, t;

	if (denom != 0.0f) {
		s = (b * e - c * d) / denom;
		t = (a * e - b * d) / denom;
		s = (std::max)(0.0f, (std::min)(1.0f, s));
		t = (std::max)(0.0f, (std::min)(1.0f, t));
	}
	else {
		s = 0.0f;
		t = d / b;
		t = (std::max)(0.0f, (std::min)(1.0f, t));
	}

	Vector3 closestPoint1 = seg1.origin + u * s;
	Vector3 closestPoint2 = seg2.origin + v * t;
	return (closestPoint1 - closestPoint2).LengthSq();
}

//
static Vector3 ClosestPointSegmentSegment(const Segment& seg1, const Segment& seg2, Vector3 currentClosest) {
	Vector3 u = seg1.diff();
	Vector3 v = seg2.diff();
	Vector3 w = Subtract(seg1.origin, seg2.origin);

	float a = Dot(u, u);
	float b = Dot(u, v);
	float c = Dot(v, v);
	float d = Dot(u, w);
	float e = Dot(v, w);

	float denom = a * c - b * b;

	// ゼロ除算の回避
	float s = 0.0f, t = 0.0f;
	if (denom != 0.0f) {
		s = Clamp3((b * e - c * d) / denom, 0.0f, 1.0f);
		t = Clamp3((a * e - b * d) / denom, 0.0f, 1.0f);
	}

	Vector3 closestOnSeg1 = Add(seg1.origin, Multiply(u, s));
	Vector3 closestOnSeg2 = Add(seg2.origin, Multiply(v, t));

	return LengthSquared(Subtract(closestOnSeg1, closestOnSeg2)) < LengthSquared(Subtract(currentClosest, seg1.origin))
		? closestOnSeg1
		: currentClosest;
}

// 
static Vector3 ClosestPointSegmentTriangle(const Segment& segment, const Triangle& triangle) {
	// 三角形の3辺を定義
	Segment edge1 = { triangle.vertices[0], triangle.vertices[1] };
	Segment edge2 = { triangle.vertices[1], triangle.vertices[2] };
	Segment edge3 = { triangle.vertices[2], triangle.vertices[0] };

	// 最初に線分の始点を初期最近接点とする
	Vector3 closest = ClosestPointSegment(segment, triangle.vertices[0]);

	// 三角形のエッジに対して最近接点を更新
	closest = ClosestPointSegmentSegment(segment, edge1, closest);
	closest = ClosestPointSegmentSegment(segment, edge2, closest);
	closest = ClosestPointSegmentSegment(segment, edge3, closest);

	// 三角形の面上の最近接点も考慮（重要）
	Plane plane = PlaneFromPoints(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]);
	Vector3 pointOnPlane = ClosestPointOnPlane(plane, segment.origin);
	closest = LengthSquared(Subtract(closest, segment.origin)) < LengthSquared(Subtract(pointOnPlane, segment.origin))
		? closest
		: pointOnPlane;

	return closest;
}

//
static Vector3 ClosestPointSegment(const Segment& segment, const Vector3& point) {
	Vector3 diff = segment.diff();
	float lenSq = Dot(diff, diff);

	if (lenSq == 0.0f) {
		return segment.origin;  // 線分が点の場合は始点を返す
	}

	float t = Dot(Subtract(point, segment.origin), diff) / lenSq;
	t = Clamp3(t, 0.0f, 1.0f);

	return Add(segment.origin, Multiply(diff, t));
}

// 
static float SegmentClosestDistanceSq(const Segment& seg0, const Segment& seg1) {
	Vector3 u = seg0.diff();
	Vector3 v = seg1.diff();
	Vector3 w = seg0.origin - seg1.origin;

	float a = u.Dot(u); // seg0の長さの2乗
	float b = u.Dot(v);
	float c = v.Dot(v); // seg1の長さの2乗
	float d = u.Dot(w);
	float e = v.Dot(w);

	float denom = a * c - b * b;
	float sN, sD = denom;
	float tN, tD = denom;

	// デノミネーターがゼロの場合（平行な場合）、近似値をとる
	if (denom < 1e-6f) {
		sN = 0.0f;
		sD = 1.0f;
		tN = e;
		tD = c;
	}
	else {
		sN = (b * e - c * d);
		tN = (a * e - b * d);

		if (sN < 0.0f) {
			sN = 0.0f;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0f) {
		tN = 0.0f;
		if (-d < 0.0f) {
			sN = 0.0f;
		}
		else if (-d > a) {
			sN = sD;
		}
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {
		tN = tD;
		if ((-d + b) < 0.0f) {
			sN = 0.0f;
		}
		else if ((-d + b) > a) {
			sN = sD;
		}
		else {
			sN = (-d + b);
			sD = a;
		}
	}

	float sc = (std::abs(sN) < 1e-6f ? 0.0f : sN / sD);
	float tc = (std::abs(tN) < 1e-6f ? 0.0f : tN / tD);

	Vector3 dP = w + (u * sc) - (v * tc);
	return dP.LengthSq();
}


// アーク長を計算する関数
static float CalculateArcLength(const std::vector<Vector3>& controlPoints, int numSamples) {
	float totalLength = 0.0f;
	Vector3 prevPos = CatmullRom(controlPoints, 0.0f);

	for (int i = 1; i <= numSamples; ++i) {
		float t = static_cast<float>(i) / numSamples;
		Vector3 currPos = CatmullRom(controlPoints, t);
		totalLength += Distance(prevPos, currPos);
		prevPos = currPos;
	}

	return totalLength;
}

// アーク長に基づく位置を取得する関数
static float FindTByArcLength(const std::vector<Vector3>& controlPoints, float targetLength, int numSamples) {
	float currentLength = 0.0f;
	Vector3 prevPos = CatmullRom(controlPoints, 0.0f);

	for (int i = 1; i <= numSamples; ++i) {
		float t = static_cast<float>(i) / numSamples;
		Vector3 currPos = CatmullRom(controlPoints, t);
		currentLength += Distance(prevPos, currPos);

		if (currentLength >= targetLength) {
			return t;
		}
		prevPos = currPos;
	}

	return 1.0f; // 最後まで到達した場合
}


// 曲線を細かくサンプリングし、累積アーク長を計算
static std::vector<std::pair<float, float>> CalculateArcLengths(const std::vector<Vector3>& controlPoints, int numSamples) {
	std::vector<std::pair<float, float>> arcLengths;
	arcLengths.push_back({ 0.0f, 0.0f });

	float totalLength = 0.0f;
	Vector3 prevPos = CatmullRom(controlPoints, 0.0f);

	for (int i = 1; i <= numSamples; ++i) {
		float t = static_cast<float>(i) / numSamples;
		Vector3 currPos = CatmullRom(controlPoints, t);
		totalLength += Distance(prevPos, currPos);
		arcLengths.push_back({ t, totalLength });
		prevPos = currPos;
	}

	return arcLengths;
}

// アーク長からtを逆算する関数
static float GetTFromArcLength(const std::vector<std::pair<float, float>>& arcLengths, float targetLength) {
	for (size_t i = 1; i < arcLengths.size(); ++i) {
		if (arcLengths[i].second >= targetLength) {
			float t1 = arcLengths[i - 1].first;
			float t2 = arcLengths[i].first;
			float l1 = arcLengths[i - 1].second;
			float l2 = arcLengths[i].second;

			// 線形補間でtを求める
			return t1 + (targetLength - l1) / (l2 - l1) * (t2 - t1);
		}
	}
	return 1.0f;
}

// 曲率を計算する関数
static float Curvature(const Vector3& p0, const Vector3& p1, const Vector3& p2) {
	Vector3 v1 = Subtract(p1, p0);
	Vector3 v2 = Subtract(p2, p1);
	float angle = std::acos(Dot(Normalize(v1), Normalize(v2)));
	return angle / Distance(p0, p1);
}

#pragma endregion
