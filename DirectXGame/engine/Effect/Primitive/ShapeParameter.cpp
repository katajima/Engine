#include "ShapeParameter.h"

void ShapeParameter::ShapePlane::Create(ModelMesh* mesh, bool isInv)
{
	mesh->Clear();

	float normal = 1.0f;
	if (isInv) {
		normal = -1.0f;
	}
	mesh->vertices.push_back({ .position = {width,height,0.0f,1.0f}  ,.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,normal } });	// 左上
	mesh->vertices.push_back({ .position = {-width,height,0.0f,1.0f} ,.texcoord = {0.0f,0.0f},.normal = {0.0f,0.0f,normal } });	// 右上
	mesh->vertices.push_back({ .position = {width,-height,0.0f,1.0f} ,.texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,normal } });	// 左下
	mesh->vertices.push_back({ .position = {-width,-height,0.0f,1.0f},.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,normal } });	// 右下

	mesh->indices.push_back(0);
	mesh->indices.push_back(1);
	mesh->indices.push_back(2);
	mesh->indices.push_back(2);
	mesh->indices.push_back(1);
	mesh->indices.push_back(3);

	mesh->UpdateVertexBuffer();
	mesh->UpdateIndexBuffer();
}

void ShapeParameter::ShapeCross::Create(ModelMesh* mesh)
{
	// 既存の頂点とインデックスをクリア
	mesh->Clear();

	float halfArmLength = armLength / 2.0f;
	float halfArmWidth = armWidth / 2.0f;

	// 頂点データの生成（12頂点）
	mesh->vertices.push_back({ .position = {-halfArmWidth,  halfArmLength, 0.0f, 1.0f}, .texcoord = {0.25f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 上左
	mesh->vertices.push_back({ .position = { halfArmWidth,  halfArmLength, 0.0f, 1.0f}, .texcoord = {0.75f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 上右
	mesh->vertices.push_back({ .position = { halfArmWidth,  halfArmWidth, 0.0f, 1.0f}, .texcoord = {0.75f, 0.25f}, .normal = {0.0f, 0.0f, 1.0f} }); // 中央上右
	mesh->vertices.push_back({ .position = { halfArmLength, halfArmWidth, 0.0f, 1.0f}, .texcoord = {1.0f, 0.25f}, .normal = {0.0f, 0.0f, 1.0f} }); // 中央右上
	mesh->vertices.push_back({ .position = { halfArmLength, -halfArmWidth, 0.0f, 1.0f}, .texcoord = {1.0f, 0.75f}, .normal = {0.0f, 0.0f, 1.0f} }); // 中央右下
	mesh->vertices.push_back({ .position = { halfArmWidth, -halfArmWidth, 0.0f, 1.0f}, .texcoord = {0.75f, 0.75f}, .normal = {0.0f, 0.0f, 1.0f} }); // 中央下右
	mesh->vertices.push_back({ .position = { halfArmWidth, -halfArmLength, 0.0f, 1.0f}, .texcoord = {0.75f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 下右
	mesh->vertices.push_back({ .position = {-halfArmWidth, -halfArmLength, 0.0f, 1.0f}, .texcoord = {0.25f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 下左
	mesh->vertices.push_back({ .position = {-halfArmWidth, -halfArmWidth, 0.0f, 1.0f}, .texcoord = {0.25f, 0.75f}, .normal = {0.0f, 0.0f, 1.0f} }); // 中央下左
	mesh->vertices.push_back({ .position = {-halfArmLength, -halfArmWidth, 0.0f, 1.0f}, .texcoord = {0.0f, 0.75f}, .normal = {0.0f, 0.0f, 1.0f} }); // 中央左下
	mesh->vertices.push_back({ .position = {-halfArmLength, halfArmWidth, 0.0f, 1.0f}, .texcoord = {0.0f, 0.25f}, .normal = {0.0f, 0.0f, 1.0f} }); // 中央左上
	mesh->vertices.push_back({ .position = {-halfArmWidth, halfArmWidth, 0.0f, 1.0f}, .texcoord = {0.25f, 0.25f}, .normal = {0.0f, 0.0f, 1.0f} }); // 中央上左

	// インデックスデータの生成
	// 上のアーム
	mesh->indices.push_back(0); mesh->indices.push_back(1); mesh->indices.push_back(2);
	mesh->indices.push_back(0); mesh->indices.push_back(2); mesh->indices.push_back(11);

	// 右のアーム
	mesh->indices.push_back(2); mesh->indices.push_back(3); mesh->indices.push_back(4);
	mesh->indices.push_back(2); mesh->indices.push_back(4); mesh->indices.push_back(5);

	// 下のアーム
	mesh->indices.push_back(5); mesh->indices.push_back(7); mesh->indices.push_back(8);
	mesh->indices.push_back(5); mesh->indices.push_back(6); mesh->indices.push_back(7);

	// 左のアーム
	mesh->indices.push_back(8); mesh->indices.push_back(9); mesh->indices.push_back(10);
	mesh->indices.push_back(8); mesh->indices.push_back(10); mesh->indices.push_back(11);

	// 中央の四角形
	mesh->indices.push_back(2); mesh->indices.push_back(5); mesh->indices.push_back(8);
	mesh->indices.push_back(2); mesh->indices.push_back(8); mesh->indices.push_back(11);

	mesh->UpdateVertexBuffer();
	mesh->UpdateIndexBuffer();
}

void ShapeParameter::ShapeTriangle::Create(ModelMesh* mesh)
{
	mesh->Clear();

	mesh->vertices.push_back({ .position = {upPos.x,upPos.y,0.0f,1.0f} ,.texcoord = {0.5f,0.0f},.normal = {0.0f,0.0f,1.0f } });	// 上
	mesh->vertices.push_back({ .position = {rightPos.x,rightPos.y,0.0f,1.0f} ,.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f } });	// 右下
	mesh->vertices.push_back({ .position = {leftPos.x,leftPos.y,0.0f,1.0f} ,.texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f } });	// 左下

	mesh->indices.push_back(0);
	mesh->indices.push_back(2);
	mesh->indices.push_back(1);

	mesh->UpdateVertexBuffer();
	mesh->UpdateIndexBuffer();
}

void ShapeParameter::Circle::Create(ModelMesh* mesh)
{
	if (segments < 3) {
		segments = 3;
	}

	mesh->Clear();

	// 中心の頂点
	mesh->vertices.push_back({
		.position = {0.0f, 0.0f, 0.0f, 1.0f},  // 中心座標
		.texcoord = {0.5f, 0.5f},              // UV中心
		.normal = {0.0f, 0.0f, 1.0f}           // 法線
		});

	// 円周の頂点
	for (int i = 0; i <= segments; ++i) {
		float theta = 2.0f * float(M_PI) * i / segments; // 角度を計算
		float x = radius * cos(theta);
		float y = radius * sin(theta);

		mesh->vertices.push_back({
			.position = {x, y, 0.0f, 1.0f},       // 円周の座標
			.texcoord = {(x / (2.0f * radius) + 0.5f), (y / (2.0f * radius) + 0.5f)}, // UV座標
			.normal = {0.0f, 0.0f, 1.0f}          // 法線
			});
	}

	// インデックスの設定
	for (int i = 1; i <= segments; ++i) {
		mesh->indices.push_back(0);       // 中心点
		mesh->indices.push_back(i + 1);   // 次の頂点
		mesh->indices.push_back(i);       // 現在の頂点
	}

	mesh->UpdateVertexBuffer();
	mesh->UpdateIndexBuffer();
}

void ShapeParameter::Star::Create(ModelMesh* mesh)
{
	if (segments < 3) {
		segments = 3;
	}
	if (segments < 3) {
		throw std::invalid_argument("Segments must be >= 3.");
	}

	// 既存の頂点とインデックスをクリア
	mesh->Clear();

	// 中心の頂点を追加
	mesh->vertices.push_back({
		.position = {0.0f, 0.0f, 0.0f, 1.0f},
		.texcoord = {0.5f, 0.5f},
		.normal = {0.0f, 0.0f, -1.0f}
		});

	// 頂点生成
	for (int i = 0; i < segments * 2; ++i) {
		float theta = float(M_PI) * i / segments;
		if (segments % 2 != 0) {
			theta -= (float(M_PI) / segments); // 奇数の場合、角度を調整
		}
		float radius = (i % 2 == 0) ? outerRadius : innerRadius;
		float x = radius * cos(theta);
		float y = radius * sin(theta);

		mesh->vertices.push_back({
			.position = {x, y, 0.0f, 1.0f},
			.texcoord = {x / (2.0f * outerRadius) + 0.5f, -y / (2.0f * outerRadius) + 0.5f},
			.normal = {0.0f, 0.0f, -1.0f}
			});
	}

	// インデックス生成（面の向きを反転）
	for (int i = 1; i <= segments * 2; ++i) {
		mesh->indices.push_back(0);                                   // 中心点
		mesh->indices.push_back(i);                                   // 現在の頂点
		mesh->indices.push_back((i % (segments * 2)) + 1);            // 次の頂点（ループ処理）
	}

	mesh->UpdateVertexBuffer();
	mesh->UpdateIndexBuffer();
}

void ShapeParameter::Crescent::Create(ModelMesh* mesh)
{
	if (segments < 3) {
		segments = 3;
	}
	if (innerRadius >= outerRadius) {
		innerRadius = outerRadius;
	}
	if (distance <= 0) {
		distance = 0;
	}
	if (segments < 3 || innerRadius > outerRadius || distance < 0) {
		throw std::invalid_argument("Invalid parameters.");
	}

	// 既存の頂点とインデックスをクリア
	mesh->Clear();

	// 外側の円の頂点を生成
	for (int i = 0; i <= segments; ++i) {
		float theta = 2.0f * float(M_PI) * i / segments;
		float x = outerRadius * cos(theta);
		float y = outerRadius * sin(theta);

		mesh->vertices.push_back({
			.position = {x, y, 0.0f, 1.0f},
			.texcoord = {0.5f + 0.5f * cos(theta), 0.5f + 0.5f * sin(theta)},
			.normal = {0.0f, 0.0f, -1.0f} // 法線を逆に設定
			});
	}

	// 内側の円の頂点を生成
	for (int i = 0; i <= segments; ++i) {
		float theta = 2.0f * float(M_PI) * i / segments;
		float x = innerRadius * cos(theta) + distance;
		float y = innerRadius * sin(theta);

		mesh->vertices.push_back({
			.position = {x, y, 0.0f, 1.0f},
			.texcoord = {0.5f + 0.5f * cos(theta), 0.5f + 0.5f * sin(theta)},
			.normal = {0.0f, 0.0f, -1.0f} // 法線を逆に設定
			});
	}

	// クレセント形状のインデックス生成
	int outerOffset = 0;
	int innerOffset = segments + 1;

	for (int i = 0; i < segments; ++i) {
		int next = (i + 1) % (segments + 1);

		// 外側の円の三角形（反転）
		mesh->indices.push_back(innerOffset + i);
		mesh->indices.push_back(outerOffset + next);
		mesh->indices.push_back(outerOffset + i);

		mesh->indices.push_back(innerOffset + i);
		mesh->indices.push_back(innerOffset + next);
		mesh->indices.push_back(outerOffset + next);
	}

	mesh->UpdateVertexBuffer();
	mesh->UpdateIndexBuffer();
}

void ShapeParameter::Ring::Create(ModelMesh* mesh)
{
	if (segments < 3) {
		segments = 3;
	}
	if (innerRadius >= outerRadius) {
		innerRadius = outerRadius;
	}
	if (innerRadius > outerRadius || segments < 3) {
		throw std::invalid_argument("Invalid parameters: innerRadius must be smaller than outerRadius, and segments must be >= 3.");
	}

	// 既存の頂点とインデックスをクリア
	mesh->Clear();

	float segmentsF = static_cast<float>(segments);

	const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / segmentsF;
	for (int index = 0; index < segments; ++index) {
		float indexF = static_cast<float>(index);
		float sin = std::sinf(indexF * radianPerDivide);
		float cos = std::cosf(indexF * radianPerDivide);
		float sinNext = std::sinf((indexF + 1) * radianPerDivide);
		float cosNext = std::cosf((indexF + 1) * radianPerDivide);
		float u = indexF / segmentsF;
		float uNext = (indexF + 1) / segmentsF;

		// {ポジション,UV,法線}
		mesh->vertices.push_back({ Vector4{ -sin * outerRadius, cos * outerRadius,0.0f,1.0f},Vector2{u ,0.0f},Vector3{0.0f,0.0f ,1.0f} });            // ①
		mesh->vertices.push_back({ Vector4{ -sinNext * outerRadius, cosNext * outerRadius,0.0f,1.0f},Vector2{uNext ,0.0f},Vector3{0.0f,0.0f ,1.0f} });// ②
		mesh->vertices.push_back({ Vector4{ -sin * innerRadius, cos * innerRadius,0.0f,1.0f},Vector2{u ,1.0f},Vector3{0.0f,0.0f ,1.0f} });            // ③
		mesh->vertices.push_back({ Vector4{ -sinNext * innerRadius, cosNext * innerRadius,0.0f,1.0f},Vector2{uNext ,1.0f},Vector3{0.0f,0.0f ,1.0f} });// ④


		// 現在セグメントの先頭頂点のインデックスを計算
		int baseIndex = index * 4;

		// 三角形1: 外側現在点 (①)、外側次点 (②)、内側現在点 (③)
		mesh->indices.push_back(baseIndex);
		mesh->indices.push_back(baseIndex + 1);
		mesh->indices.push_back(baseIndex + 2);

		// 三角形2: 外側次点 (②)、内側次点 (④)、内側現在点 (③)
		mesh->indices.push_back(baseIndex + 1);
		mesh->indices.push_back(baseIndex + 3);
		mesh->indices.push_back(baseIndex + 2);
	}

	mesh->UpdateVertexBuffer();
	mesh->UpdateIndexBuffer();
}

void ShapeParameter::ShapeSphere::Create(ModelMesh* mesh)
{
	if (longitudeSegments < 3) {
		longitudeSegments = 3;
	}
	if (latitudeSegments < 2) {
		latitudeSegments = 2;
	}
	if (latitudeSegments < 2 || longitudeSegments < 3) {
		throw std::invalid_argument("Latitude segments must be >= 2 and longitude segments must be >= 3.");
	}

	mesh->Clear();

	// 頂点生成
	for (int lat = 0; lat <= latitudeSegments; ++lat) {
		float theta = float(M_PI) * lat / latitudeSegments; // 緯度（0 ～ π）
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		// UV の v 計算（基準切り替え）
		float v = isTopBased ? (1.0f - static_cast<float>(lat) / latitudeSegments)  // 上基準
			: static_cast<float>(lat) / latitudeSegments;         // 下基準

		for (int lon = 0; lon <= longitudeSegments; ++lon) {
			float phi = 2.0f * float(M_PI) * lon / longitudeSegments; // 経度（0 ～ 2π）
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			// Y 軸基準の球体座標
			float x = radius * sinTheta * cosPhi;
			float z = radius * sinTheta * sinPhi;
			float y = radius * cosTheta;

			// UV座標
			float u = static_cast<float>(lon) / longitudeSegments;

			// 頂点追加
			mesh->vertices.push_back({
				.position = {x, y, z, 1.0f},   // 頂点座標
				.texcoord = {u, v},            // UV座標
				.normal = {sinTheta * cosPhi, cosTheta, sinTheta * sinPhi} // 法線
				});
		}
	}

	// インデックス生成
	for (int lat = 0; lat < latitudeSegments; ++lat) {
		for (int lon = 0; lon < longitudeSegments; ++lon) {
			int first = (lat * (longitudeSegments + 1)) + lon;
			int second = first + longitudeSegments + 1;

			// 三角形1（インデックス順序を反転）
			mesh->indices.push_back(first + 1);
			mesh->indices.push_back(second);
			mesh->indices.push_back(first);

			// 三角形2（インデックス順序を反転）
			mesh->indices.push_back(first + 1);
			mesh->indices.push_back(second + 1);
			mesh->indices.push_back(second);

		}
	}

	mesh->UpdateVertexBuffer();
	mesh->UpdateIndexBuffer();
}

void ShapeParameter::ShapeArrow::Create(ModelMesh* mesh)
{
	if (segments < 3) {
		segments = 3;
	}
	if (segments < 3) {
		throw std::invalid_argument("Segments must be >= 3.");
	}

	// 既存の頂点とインデックスをクリア
	mesh->Clear();

	// 矢のシャフトの生成（円柱）
	float halfShaftLength = shaftLength / 2.0f;
	for (int i = 0; i <= segments; ++i) {
		float theta = 2.0f * float(M_PI) * i / segments;
		float x = shaftRadius * cos(theta);
		float z = shaftRadius * sin(theta);

		// 上部の頂点
		mesh->vertices.push_back({
			.position = {x, halfShaftLength, z, 1.0f},
			.texcoord = {static_cast<float>(i) / segments, 0.0f},
			.normal = {x / shaftRadius, 0.0f, z / shaftRadius}
			});

		// 下部の頂点
		mesh->vertices.push_back({
			.position = {x, -halfShaftLength, z, 1.0f},
			.texcoord = {static_cast<float>(i) / segments, 1.0f},
			.normal = {x / shaftRadius, 0.0f, z / shaftRadius}
			});
	}

	// 矢のシャフトのインデックス生成（反転）
	for (int i = 0; i < segments; ++i) {
		int top1 = i * 2;
		int top2 = (i + 1) * 2;
		int bottom1 = i * 2 + 1;
		int bottom2 = (i + 1) * 2 + 1;

		// 三角形を生成
		mesh->indices.push_back(top2);
		mesh->indices.push_back(bottom1);
		mesh->indices.push_back(top1);

		mesh->indices.push_back(bottom2);
		mesh->indices.push_back(bottom1);
		mesh->indices.push_back(top2);
	}

	// 矢の頭の生成（円錐）
	float headBaseY = halfShaftLength;
	float headTipY = headBaseY + headLength;
	for (int i = 0; i <= segments; ++i) {
		float theta = 2.0f * float(M_PI) * i / segments;
		float x = headRadius * cos(theta);
		float z = headRadius * sin(theta);

		// 頂点を追加
		mesh->vertices.push_back({
			.position = {x, headBaseY, z, 1.0f},
			.texcoord = {static_cast<float>(i) / segments, 1.0f},
			.normal = {x / headRadius, 0.0f, z / headRadius}
			});
	}

	// 頭の頂点（先端）
	mesh->vertices.push_back({
		.position = {0.0f, headTipY, 0.0f, 1.0f},
		.texcoord = {0.5f, 0.0f}, // UV座標を先端に集める
		.normal = {0.0f, 1.0f, 0.0f}
		});

	// 矢の頭のインデックス生成（反転）
	int headBaseOffset = (segments + 1) * 2;
	int headTipIndex = (int)mesh->vertices.size() - 1;
	for (int i = 0; i < segments; ++i) {
		int base1 = headBaseOffset + i;
		int base2 = headBaseOffset + i + 1;

		// 頭の三角形を生成

		mesh->indices.push_back(base2);
		mesh->indices.push_back(base1);
		mesh->indices.push_back(headTipIndex);

	}

	mesh->UpdateVertexBuffer();
	mesh->UpdateIndexBuffer();
}

void ShapeParameter::ShapeCube::Create(ModelMesh* mesh)
{
	// 既存の頂点とインデックスをクリア
	mesh->Clear();

	// 各面の法線
	Vector3 normals[] = {
		{ 0.0f,  0.0f,  1.0f}, // 前面
		{ 0.0f,  0.0f, -1.0f}, // 背面
		{ 0.0f,  1.0f,  0.0f}, // 上面
		{ 0.0f, -1.0f,  0.0f}, // 底面
		{ 1.0f,  0.0f,  0.0f}, // 右側面
		{-1.0f,  0.0f,  0.0f}  // 左側面
	};

	//Vector3 size = size * 1.0f;

	// 各面の頂点座標 (1つの面に4頂点)
	Vector4 positions[][4] = {
		// 前面
		{{ size.x,  size.y,  size.z, 1.0f}, {-size.x,  size.y,  size.z, 1.0f}, { size.x, -size.y,  size.z, 1.0f}, {-size.x, -size.y,  size.z, 1.0f}},
		// 背面
		{{ size.x,  size.y, -size.z, 1.0f}, { size.x, -size.y, -size.z, 1.0f}, {-size.x,  size.y, -size.z, 1.0f}, {-size.x, -size.y, -size.z, 1.0f}},
		// 上面
		{{ size.x,  size.y, -size.z, 1.0f}, {-size.x,  size.y, -size.z, 1.0f}, { size.x,  size.y,  size.z, 1.0f}, {-size.x,  size.y,  size.z, 1.0f}},
		// 底面
		{{ size.x, -size.y,  size.z, 1.0f}, {-size.x, -size.y,  size.z, 1.0f}, { size.x, -size.y, -size.z, 1.0f}, {-size.x, -size.y, -size.z, 1.0f}},
		// 右側面
		{{ size.x,  size.y, -size.z, 1.0f}, { size.x,  size.y,  size.z, 1.0f}, { size.x, -size.y, -size.z, 1.0f}, { size.x, -size.y,  size.z, 1.0f}},
		// 左側面
		{{-size.x,  size.y,  size.z, 1.0f}, {-size.x,  size.y, -size.z, 1.0f}, {-size.x, -size.y,  size.z, 1.0f}, {-size.x, -size.y, -size.z, 1.0f}}
	};

	// 各面のUV座標
	Vector2 texcoords[] = {
		{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}
	};

	// 各面のインデックスオフセット
	int vertexOffset = 0;

	for (int i = 0; i < 6; ++i) { // 6面
		// 頂点データを追加
		for (int j = 0; j < 4; ++j) { // 各面の4頂点
			mesh->vertices.push_back({
				positions[i][j],      // 座標
				texcoords[j],         // UV
				normals[i]            // 法線
				});
		}

		// インデックスデータを追加 (2つの三角形)
		mesh->indices.push_back(vertexOffset + 0);
		mesh->indices.push_back(vertexOffset + 1);
		mesh->indices.push_back(vertexOffset + 2);

		mesh->indices.push_back(vertexOffset + 2);
		mesh->indices.push_back(vertexOffset + 1);
		mesh->indices.push_back(vertexOffset + 3);

		vertexOffset += 4; // 次の面に移動
	}


	mesh->UpdateVertexBuffer();
	mesh->UpdateIndexBuffer();
}

void ShapeParameter::Cylinder::Create(ModelMesh* mesh)
{
	if (segments < 3) {
		segments = 3;
	}
	if (segments < 3) {
		throw std::invalid_argument("Segments must be >= 3.");
	}

	// 頂点とインデックスをクリア
	mesh->Clear();

	const float radianPerDivide = 2.0f * std::numbers::pi_v<float> / static_cast<float>(segments);
	const float halfHeight = height * 0.5f;

	// 側面の頂点とインデックスを生成
	for (int index = 0; index < segments; ++index) {
		float sin = std::sin(index * radianPerDivide);
		float cos = std::cos(index * radianPerDivide);
		float sinNext = std::sin((index + 1) * radianPerDivide);
		float cosNext = std::cos((index + 1) * radianPerDivide);
		float u = float(index) / float(segments);
		float uNext = float(index + 1) / float(segments);

		// 側面の頂点
		mesh->vertices.push_back({ Vector4{ sin * innerRadius, cos * innerRadius, -halfHeight, 1.0f }, Vector2{ u, 0.0f }, Vector3{ sin, cos, 0.0f } });
		mesh->vertices.push_back({ Vector4{ sinNext * innerRadius, cosNext * innerRadius, -halfHeight, 1.0f }, Vector2{ uNext, 0.0f }, Vector3{ sinNext, cosNext, 0.0f } });
		mesh->vertices.push_back({ Vector4{ sin * outerRadius, cos * outerRadius, halfHeight, 1.0f }, Vector2{ u, 1.0f }, Vector3{ sin, cos, 0.0f } });
		mesh->vertices.push_back({ Vector4{ sinNext * outerRadius, cosNext * outerRadius, halfHeight, 1.0f }, Vector2{ uNext, 1.0f }, Vector3{ sinNext, cosNext, 0.0f } });

		int baseIndex = index * 4;
		mesh->indices.push_back(baseIndex + 0);
		mesh->indices.push_back(baseIndex + 2);
		mesh->indices.push_back(baseIndex + 1);

		mesh->indices.push_back(baseIndex + 1);
		mesh->indices.push_back(baseIndex + 2);
		mesh->indices.push_back(baseIndex + 3);
	}

	if (isCover) {
		// 天面の生成 (時計回り)
		for (int index = 0; index < segments; ++index) {
			float sin = std::sin(index * radianPerDivide);
			float cos = std::cos(index * radianPerDivide);
			float sinNext = std::sin((index + 1) * radianPerDivide);
			float cosNext = std::cos((index + 1) * radianPerDivide);

			// 天面の頂点
			mesh->vertices.push_back({ Vector4{ 0.0f, 0.0f, halfHeight, 1.0f }, Vector2{ 0.5f, 0.5f }, Vector3{ 0.0f, 0.0f, 1.0f } });
			mesh->vertices.push_back({ Vector4{ sin * outerRadius, cos * outerRadius, halfHeight, 1.0f }, Vector2{ sin * 0.5f + 0.5f, cos * 0.5f + 0.5f }, Vector3{ 0.0f, 0.0f, 1.0f } });
			mesh->vertices.push_back({ Vector4{ sinNext * outerRadius, cosNext * outerRadius, halfHeight, 1.0f }, Vector2{ sinNext * 0.5f + 0.5f, cosNext * 0.5f + 0.5f }, Vector3{ 0.0f, 0.0f, 1.0f } });

			int topBaseIndex = segments * 4 + index * 3;
			mesh->indices.push_back(topBaseIndex + 0); // 中央点
			mesh->indices.push_back(topBaseIndex + 2);
			mesh->indices.push_back(topBaseIndex + 1);
		}

		// 底面の生成 (反時計回りに反転)
		for (int index = 0; index < segments; ++index) {
			float sin = std::sin(index * radianPerDivide);
			float cos = std::cos(index * radianPerDivide);
			float sinNext = std::sin((index + 1) * radianPerDivide);
			float cosNext = std::cos((index + 1) * radianPerDivide);

			// 底面の頂点
			mesh->vertices.push_back({ Vector4{ 0.0f, 0.0f, -halfHeight, 1.0f }, Vector2{ 0.5f, 0.5f }, Vector3{ 0.0f, 0.0f, -1.0f } });
			mesh->vertices.push_back({ Vector4{ sin * innerRadius, cos * innerRadius, -halfHeight, 1.0f }, Vector2{ sin * 0.5f + 0.5f, cos * 0.5f + 0.5f }, Vector3{ 0.0f, 0.0f, -1.0f } });
			mesh->vertices.push_back({ Vector4{ sinNext * innerRadius, cosNext * innerRadius, -halfHeight, 1.0f }, Vector2{ sinNext * 0.5f + 0.5f, cosNext * 0.5f + 0.5f }, Vector3{ 0.0f, 0.0f, -1.0f } });

			int bottomBaseIndex = segments * 4 + segments * 3 + index * 3;
			mesh->indices.push_back(bottomBaseIndex + 0); // 中央点
			mesh->indices.push_back(bottomBaseIndex + 1);
			mesh->indices.push_back(bottomBaseIndex + 2);
		}
	}

	// バッファの更新
	mesh->UpdateVertexBuffer();
	mesh->UpdateIndexBuffer();
}

void ShapeParameter::Tube::Create(ModelMesh* mesh)
{
	if (segments < 3) {
		segments = 3;
	}
	if (segments < 3) {
		throw std::invalid_argument("Segments must be >= 3.");
	}

	float halfHeight = height / 2.0f;

	// 既存の頂点とインデックスをクリア
	mesh->Clear();

	// 頂点データの生成
	for (int i = 0; i <= segments; ++i) {
		float theta = 2.0f * float(M_PI) * i / segments;
		float x_outer = radius * cos(theta);
		float z_outer = radius * sin(theta);
		float x_inner = innerRadius * cos(theta);
		float z_inner = innerRadius * sin(theta);

		// 外側の頂点
		mesh->vertices.push_back({
			.position = {x_outer, halfHeight, z_outer, 1.0f},
			.texcoord = {static_cast<float>(i) / segments, 0.0f},
			.normal = {x_outer / radius, 0.0f, z_outer / radius}
			});

		mesh->vertices.push_back({
			.position = {x_outer, -halfHeight, z_outer, 1.0f},
			.texcoord = {static_cast<float>(i) / segments, 1.0f},
			.normal = {x_outer / radius, 0.0f, z_outer / radius}
			});

		// 内側の頂点
		mesh->vertices.push_back({
			.position = {x_inner, halfHeight, z_inner, 1.0f},
			.texcoord = {static_cast<float>(i) / segments, 0.0f},
			.normal = {-x_inner / innerRadius, 0.0f, -z_inner / innerRadius} // 法線を逆に
			});

		mesh->vertices.push_back({
			.position = {x_inner, -halfHeight, z_inner, 1.0f},
			.texcoord = {static_cast<float>(i) / segments, 1.0f},
			.normal = {-x_inner / innerRadius, 0.0f, -z_inner / innerRadius} // 法線を逆に
			});
	}

	// 側面のインデックス生成
	for (int i = 0; i < segments; ++i) {
		int topOuter1 = i * 4;          // 外側上の現在の頂点
		int topOuter2 = (i + 1) * 4;    // 外側上の次の頂点
		int bottomOuter1 = i * 4 + 1;   // 外側下の現在の頂点
		int bottomOuter2 = (i + 1) * 4 + 1; // 外側下の次の頂点

		int topInner1 = i * 4 + 2;      // 内側上の現在の頂点
		int topInner2 = (i + 1) * 4 + 2; // 内側上の次の頂点
		int bottomInner1 = i * 4 + 3;   // 内側下の現在の頂点
		int bottomInner2 = (i + 1) * 4 + 3; // 内側下の次の頂点

		// 外側の三角形（反転）
		mesh->indices.push_back(topOuter2);
		mesh->indices.push_back(bottomOuter1);
		mesh->indices.push_back(topOuter1);

		mesh->indices.push_back(topOuter2);
		mesh->indices.push_back(bottomOuter2);
		mesh->indices.push_back(bottomOuter1);

		// 内側の三角形（反転）
		mesh->indices.push_back(bottomInner1);
		mesh->indices.push_back(topInner2);
		mesh->indices.push_back(topInner1);

		mesh->indices.push_back(bottomInner2);
		mesh->indices.push_back(topInner2);
		mesh->indices.push_back(bottomInner1);

		// 外側と内側の三角形（反転）
		mesh->indices.push_back(topInner1);
		mesh->indices.push_back(topOuter2);
		mesh->indices.push_back(topOuter1);

		mesh->indices.push_back(topInner2);
		mesh->indices.push_back(topOuter2);
		mesh->indices.push_back(topInner1);

		mesh->indices.push_back(bottomOuter2);
		mesh->indices.push_back(bottomInner1);
		mesh->indices.push_back(bottomOuter1);

		mesh->indices.push_back(bottomInner2);
		mesh->indices.push_back(bottomInner1);
		mesh->indices.push_back(bottomOuter2);
	}

	// バッファの更新
	mesh->UpdateVertexBuffer();
	mesh->UpdateIndexBuffer();
}

void ShapeParameter::Pyramid::Create(ModelMesh* mesh)
{

	if (segments < 3) {
		segments = 3;
	}
	if (segments < 3) {
		throw std::invalid_argument("Segments must be >= 3.");
	}

	float halfHeight = height / 2.0f;

	// 既存の頂点とインデックスをクリア
	mesh->Clear();

	// 頂点データの生成
	// 頂点0: ピラミッドの頂点（上）
	mesh->vertices.push_back({
		.position = {0.0f, halfHeight, 0.0f, 1.0f},
		.texcoord = {0.5f, 0.5f},  // UV座標の中心を設定
		.normal = {0.0f, 1.0f, 0.0f}
		});

	// 底面の頂点
	for (int i = 0; i < segments; ++i) {
		float theta = 2.0f * float(M_PI) * i / segments;
		float x = radius * cos(theta);
		float z = radius * sin(theta);

		mesh->vertices.push_back({
			.position = {x, -halfHeight, z, 1.0f},
			.texcoord = {0.5f + 0.5f * -cos(theta), 0.5f + 0.5f * sin(theta)}, // UV座標を設定
			.normal = {0.0f, -1.0f, 0.0f}
			});
	}

	// インデックスデータの生成
	for (int i = 1; i <= segments; ++i) {
		int next = (i % segments) + 1;

		// 側面の三角形（反転）
		mesh->indices.push_back(0);   // 頂点（上）
		mesh->indices.push_back(next); // 底面の次の頂点
		mesh->indices.push_back(i);   // 底面の現在の頂点

		// 底面の三角形（反転）
		mesh->indices.push_back(next); // 次の頂点
		mesh->indices.push_back(i);    // 現在の頂点
		mesh->indices.push_back(0);    // 中心点（ここでは利用しない）
	}

	// バッファの更新
	mesh->UpdateVertexBuffer();
	mesh->UpdateIndexBuffer();
}

void ShapeParameter::Torus::Create(ModelMesh* mesh)
{
	if (tubeSegments < 3) {
		tubeSegments = 3;
	}
	if (segments < 3) {
		segments = 3;
	}
	if (tubeSegments < 3 || segments < 3) {
		throw std::invalid_argument("Segments must be >= 3.");
	}

	// 既存の頂点とインデックスをクリア
	mesh->Clear();

	// 頂点データの生成
	for (int i = 0; i <= tubeSegments; ++i) {
		float theta = 2.0f * float(M_PI) * i / tubeSegments;
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);

		for (int j = 0; j <= segments; ++j) {
			float phi = 2.0f * float(M_PI) * j / segments;
			float cosPhi = cos(phi);
			float sinPhi = sin(phi);

			float x = (outerRadius + innerRadius * cosPhi) * cosTheta;
			float y = (outerRadius + innerRadius * cosPhi) * sinTheta;
			float z = innerRadius * sinPhi;

			mesh->vertices.push_back({
				.position = {x, y, z, 1.0f},
				.texcoord = {static_cast<float>(i) / tubeSegments, static_cast<float>(j) / segments},
				.normal = {cosPhi * cosTheta, cosPhi * sinTheta, sinPhi}
				});
		}
	}

	// インデックスデータの生成
	for (int i = 0; i < tubeSegments; ++i) {
		for (int j = 0; j < segments; ++j) {
			int first = (i * (segments + 1)) + j;
			int second = first + segments + 1;

			mesh->indices.push_back(first);
			mesh->indices.push_back(second);
			mesh->indices.push_back(first + 1);

			mesh->indices.push_back(second);
			mesh->indices.push_back(second + 1);
			mesh->indices.push_back(first + 1);
		}
	}

	mesh->UpdateVertexBuffer();
	mesh->UpdateIndexBuffer();
}
