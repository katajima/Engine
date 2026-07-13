#pragma once

#include "DirectXGame/engine/struct/Vector4.h"
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/struct/Vector2.h"

// スカイボックス頂点データ
struct SkyBoxVertexData {
	Vector4 position;
};

// 頂点データ
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
	Vector4 tangent; // ← w を含める
};
// ライン頂点データ
struct LineVertexData {
	Vector4 position;
	Vector4 color;    // 色
};

// トレイル頂点データ
struct TrailVertexData {
	Vector4 position;
	Vector2 texcoord; // テクスチャ座標
	Vector3 normal;
	Vector4 color;    // 色
};


//　スクリーン頂点データ
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

