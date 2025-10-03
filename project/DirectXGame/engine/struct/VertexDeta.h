#pragma once

#include "DirectXGame/engine/struct/Vector4.h"
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/struct/Vector2.h"

struct SkyBoxVertexData {
	Vector4 position;
};
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
	Vector4 tangent; // ← w を含める
};
struct LineVertexData {
	Vector4 position;
	Vector4 color;    // 色
};

struct TrailVertexData {
	Vector4 position;
	Vector2 texcoord; // テクスチャ座標
	Vector3 normal;
	Vector4 color;    // 色
};


//頂点データ
struct ScreenVertexData {

	Vector4 position;
	Vector2 texcoord;
};


// カスタムハッシュ関数
struct VertexHash {
	size_t operator()(const VertexData& vertex) const {
		return std::hash<float>()(vertex.position.x) ^
			std::hash<float>()(vertex.position.y) ^
			std::hash<float>()(vertex.position.z);
	}
};

