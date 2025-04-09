#include "ColliderFanction2D.h"


bool PointInTriangle(const Vector2& pt, const Triangle2D& triangle) {
	const Vector2& a = triangle.vertices[0];
	const Vector2& b = triangle.vertices[1];
	const Vector2& c = triangle.vertices[2];

	Vector2 v0 = c - a;
	Vector2 v1 = b - a;
	Vector2 v2 = pt - a;

	float d00 = v0.Dot(v0);
	float d01 = v0.Dot(v1);
	float d11 = v1.Dot(v1);
	float d20 = v2.Dot(v0);
	float d21 = v2.Dot(v1);

	float denom = d00 * d11 - d01 * d01;
	if (denom == 0.0f) return false; // 三角形が退化している

	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	float u = 1.0f - v - w;

	return u >= 0.0f && v >= 0.0f && w >= 0.0f;
}

float DistancePointToSegment(const Vector2& p, const Vector2& a, const Vector2& b) {

	Vector2 ab = b - a;
	Vector2 ap = p - a;
	float abLenSq = ab.Dot(ab);
	if (abLenSq == 0.0f) return (p - a).Length(); // a == b のとき

	float t = ap.Dot(ab) / abLenSq;
	t = std::clamp(t, 0.0f, 1.0f);
	Vector2 closest = { a.x + ab.x * t, a.y + ab.y * t };
	return (p - closest).Length();
}


bool IsCollision(const Triangle2D& triangle, const Sphere2D& sphere) {
	if (PointInTriangle(sphere.center, triangle)) return true;
	if (DistancePointToSegment(sphere.center, triangle.vertices[0], triangle.vertices[1]) <= sphere.radius) return true;
	if (DistancePointToSegment(sphere.center, triangle.vertices[1], triangle.vertices[2]) <= sphere.radius) return true;
	if (DistancePointToSegment(sphere.center, triangle.vertices[2], triangle.vertices[0]) <= sphere.radius) return true;
	return false;
}

