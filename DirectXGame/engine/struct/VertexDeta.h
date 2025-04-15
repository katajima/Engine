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
};
struct LineVertexData {
	Vector4 position;
	Vector4 color;    // 色
};
