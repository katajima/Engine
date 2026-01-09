#pragma once
#include "DirectXGame/engine/math/MathFunctions.h"

#pragma region 線や曲線などの汎用的な計算

// ===========================
// ベジェ曲線（2次）
// p0, p1, p2 を制御点とする曲線上の点を求める
// t: 0.0f～1.0f の範囲
// ===========================
static Vector3 Bezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, float t) {

	// 線形補間で中間点を計算
	Vector3 p0p1 = Lerp(p0, p1, t);
	Vector3 p1p2 = Lerp(p1, p2, t);

	// 2回目の補間で最終点を求める
	Vector3 p = Lerp(p0p1, p1p2, t);

	return Lerp(p0p1, p1p2, t);
}

// ===========================
// Catmull-Rom スプライン補間
// 4つの制御点から補間点を計算する
// ===========================
static Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
	const float s = 0.5f; // スケーリング係数（Catmull-Rom 固有の値）

	float t2 = t * t;  // tの2乗
	float t3 = t2 * t; // tの3乗

	// 各係数を計算
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

	// スプライン補間による出力点
	Vector3 pt = {
		(e3.x * t3 + e2.x * t2 + e1.x * t + e0.x) * s,
		(e3.y * t3 + e2.y * t2 + e1.y * t + e0.y) * s,
		(e3.z * t3 + e2.z * t2 + e1.z * t + e0.z) * s
	};

	return pt;
}

// ===========================
// Catmull-Rom スプライン（複数制御点）
// 複数の制御点列から曲線上の位置を計算
// ===========================
static Vector3 CatmullRom(const std::vector<Vector3>& points, float t) {
	assert(points.size() >= 4 && "制御点は4点以上必要です");

	// 区間数（制御点数 - 1）
	size_t division = points.size() - 1;
	if (division == 0) return points[0]; // 例外処理

	// 各区間の長さ（0.0～1.0の範囲で均等割り）
	float areaWidth = 1.0f / division;

	// 指定されたtが属する区間を求める
	size_t index = static_cast<size_t>(t / areaWidth);
	index = Math::Clamp(index, static_cast<size_t>(0), division - 1);

	// 各点のインデックスを取得
	size_t index0 = (index == 0) ? 0 : index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;

	// 最終区間の処理
	if (index3 >= points.size()) index3 = points.size() - 1;

	// 制御点取得
	const Vector3& p0 = points[index0];
	const Vector3& p1 = points[index1];
	const Vector3& p2 = points[index2];
	const Vector3& p3 = points[index3];

	// 区間内のtを正規化
	float t_2 = static_cast<float>(t - index * areaWidth) / areaWidth;
	t_2 = Math::Clamp(t_2, 0.0f, 1.0f);

	// 4点を使って補間
	return CatmullRom(p0, p1, p2, p3, t_2);
}

// ===========================
// 点と線分の距離の二乗を求める
// ===========================
static float PointLineDistanceSquared(const Vector3& point, const Vector3& a, const Vector3& b)
{
	Vector3 ab = b - a;
	Vector3 ap = point - a;
	float abLengthSquared = ab.LengthSq();

	// 線分が点の場合
	if (abLengthSquared == 0.0f) {
		return ap.LengthSq();
	}

	// 最近傍点を求める
	float t = ap.Dot(ab) / abLengthSquared;
	t = (std::max)(0.0f, (std::min)(1.0f, t));

	Vector3 closestPoint = a + ab * t;
	return (point - closestPoint).LengthSq();
}

// ===========================
// 線分と線分の距離の二乗を求める
// ===========================
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
		// 一般ケース
		s = (b * e - c * d) / denom;
		t = (a * e - b * d) / denom;
		s = (std::max)(0.0f, (std::min)(1.0f, s));
		t = (std::max)(0.0f, (std::min)(1.0f, t));
	}
	else {
		// 平行な場合
		s = 0.0f;
		t = d / b;
		t = (std::max)(0.0f, (std::min)(1.0f, t));
	}

	Vector3 closestPoint1 = seg1.origin + u * s;
	Vector3 closestPoint2 = seg2.origin + v * t;
	return (closestPoint1 - closestPoint2).LengthSq();
}

// ===========================
// 2線分間の最短距離（平方）を求める
// ===========================
static float SegmentClosestDistanceSq(const Segment& seg0, const Segment& seg1) {
	Vector3 u = seg0.diff();
	Vector3 v = seg1.diff();
	Vector3 w = seg0.origin - seg1.origin;

	float a = u.Dot(u);
	float b = u.Dot(v);
	float c = v.Dot(v);
	float d = u.Dot(w);
	float e = v.Dot(w);

	float denom = a * c - b * b;
	float sN, sD = denom;
	float tN, tD = denom;

	// 平行な場合の処理
	if (denom < 1e-6f) {
		sN = 0.0f;
		sD = 1.0f;
		tN = e;
		tD = c;
	}
	else {
		sN = (b * e - c * d);
		tN = (a * e - b * d);

		// s が範囲外の場合の補正
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

	// t が範囲外の場合の補正
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

	// s, t を正規化
	float sc = (std::abs(sN) < 1e-6f ? 0.0f : sN / sD);
	float tc = (std::abs(tN) < 1e-6f ? 0.0f : tN / tD);

	Vector3 dP = w + (u * sc) - (v * tc);
	return dP.LengthSq();
}

// ===========================
// 曲線全体のアーク長（弧の長さ）を計算
// ===========================
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

// ===========================
// アーク長から補間パラメータtを求める
// ===========================
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

	return 1.0f; // 全体を超えた場合
}

// ===========================
// サンプリングして累積アーク長を取得
// ===========================
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

// ===========================
// アーク長に基づいてtを逆算する
// ===========================
static float GetTFromArcLength(const std::vector<std::pair<float, float>>& arcLengths, float targetLength) {
	for (size_t i = 1; i < arcLengths.size(); ++i) {
		if (arcLengths[i].second >= targetLength) {
			float t1 = arcLengths[i - 1].first;
			float t2 = arcLengths[i].first;
			float l1 = arcLengths[i - 1].second;
			float l2 = arcLengths[i].second;

			// 線形補間でtを推定
			return t1 + (targetLength - l1) / (l2 - l1) * (t2 - t1);
		}
	}
	return 1.0f;
}

// ===========================
// 曲率（カーブの曲がり具合）を求める
// ===========================
static float Curvature(const Vector3& p0, const Vector3& p1, const Vector3& p2) {
	Vector3 v1 = Subtract(p1, p0);
	Vector3 v2 = Subtract(p2, p1);
	float angle = std::acos(Dot(Normalize(v1), Normalize(v2)));
	return angle / Distance(p0, p1);
}

#pragma endregion
