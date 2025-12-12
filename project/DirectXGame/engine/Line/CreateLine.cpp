#include "CreateLine.h"
#include "combaseapi.h"
#include"DirectXGame/engine/Manager/SRV/SrvManager.h"
#include "DirectXGame/engine/Material/Material.h"
#include "DirectXGame/engine/collider/Octree/Octree.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
/// <summary>
/// 初期化
/// </summary>
/// <param name="dxCommon"></param>
void Engine::LineMeshData::Initialize(DirectXCommon* dxCommon) {

	mesh_ = std::make_unique<LineMesh>();
	mesh_->verticesline.push_back({ 0,0,0,0 });
	mesh_->verticesline.push_back({ 0,0,0,0 });
	mesh_->indices.push_back({ 0 });
	mesh_->indices.push_back({ 1 });

	mesh_->Initialize(dxCommon);

	mesh_->verticesline.clear();
	mesh_->indices.clear();
};


/// <summary>
/// 更新
/// </summary>
void Engine::LineMeshData::Update() {
	mesh_->UpdateVertexBuffer();
	mesh_->UpdateIndexBuffer();
};

/// <summary>
/// 描画
/// </summary>
void Engine::LineMeshData::Draw() {
	mesh_->DrawIndexedInstanced();
}

/// <summary>
/// メッシュデータクリア
/// </summary>
void Engine::LineMeshData::Clear() {
	mesh_->Clear(); 
	lineCount_ = 0;
};

/// <summary>
/// ライン加算
/// </summary>
/// <param name="start">視点</param>
/// <param name="end">終点</param>
/// <param name="color">色</param>
void Engine::LineMeshData::AddLine(const Vector3& start, const Vector3& end, const Vector4& color) {
	CreateLine::Line(*this, LineData{ start ,end,color });
}

/// <summary>
/// カメラ
/// </summary>
/// <param name="camera"></param>
/// <param name="color"></param>
void Engine::LineMeshData::AddCameraLine(const Camera& camera, const Vector4& color)
{
	// --- カメラのパラメータ取得 ---
	float fovY = camera.fovY_;
	float aspect = camera.aspect_;
	float nearZ = camera.nearClip_;
	float farZ = camera.farClip_;

	// --- カメラのワールド行列と位置・向き取得 ---
	Matrix4x4 worldMatrix = camera.transform_.GetWorldMatrix(); // または camera->worldMatrix_
	Vector3 camPos = Vector3(worldMatrix.m[3][0], worldMatrix.m[3][1], worldMatrix.m[3][2]);

	Vector3 right = Vector3(worldMatrix.m[0][0], worldMatrix.m[0][1], worldMatrix.m[0][2]);
	Vector3 up = Vector3(worldMatrix.m[1][0], worldMatrix.m[1][1], worldMatrix.m[1][2]);
	Vector3 forward = Vector3(worldMatrix.m[2][0], worldMatrix.m[2][1], worldMatrix.m[2][2]);

	// --- Near/Far プレーンの半分の高さ・幅を計算 ---
	float tanFovY = tanf(fovY * 0.5f);
	float nearH = tanFovY * nearZ;
	float nearW = nearH * aspect;
	float farH = tanFovY * farZ;
	float farW = farH * aspect;

	// --- 各プレーンの中心座標 ---
	Vector3 nearCenter = camPos + forward * nearZ;
	Vector3 farCenter = camPos + forward * farZ;

	// --- Near plane corners ---
	Vector3 nearTopLeft = nearCenter + (up * nearH) - (right * nearW);
	Vector3 nearTopRight = nearCenter + (up * nearH) + (right * nearW);
	Vector3 nearBottomLeft = nearCenter - (up * nearH) - (right * nearW);
	Vector3 nearBottomRight = nearCenter - (up * nearH) + (right * nearW);

	// --- Far plane corners ---
	Vector3 farTopLeft = farCenter + (up * farH) - (right * farW);
	Vector3 farTopRight = farCenter + (up * farH) + (right * farW);
	Vector3 farBottomLeft = farCenter - (up * farH) - (right * farW);
	Vector3 farBottomRight = farCenter - (up * farH) + (right * farW);

	// --- Frustum ラインを追加 ---
	// Near plane
	AddLine(nearTopLeft, nearTopRight, color);
	AddLine(nearTopRight, nearBottomRight, color);
	AddLine(nearBottomRight, nearBottomLeft, color);
	AddLine(nearBottomLeft, nearTopLeft, color);

	// Far plane
	AddLine(farTopLeft, farTopRight, color);
	AddLine(farTopRight, farBottomRight, color);
	AddLine(farBottomRight, farBottomLeft, color);
	AddLine(farBottomLeft, farTopLeft, color);

	// Connections
	AddLine(nearTopLeft, farTopLeft, color);
	AddLine(nearTopRight, farTopRight, color);
	AddLine(nearBottomLeft, farBottomLeft, color);
	AddLine(nearBottomRight, farBottomRight, color);
}

/// <summary>
/// ポイントライトのデータを追加します。
/// </summary>
/// <param name="data">追加するポイントライトのデータ。</param>
void Engine::LineMeshData::AddLightLine(const PointLightData& data) {
	constexpr int segments = 16; // 1つの円を構成するセグメント数
	const float radius = data.radius; // ポイントライトの届く距離

	Vector3 center = data.position;
	Vector4 lineColor = data.color; // ライトの色をそのまま使用

	// 3つの軸 (XY, XZ, YZ) に円を描画
	for (int axis = 0; axis < 3; ++axis) {
		for (int i = 0; i < segments; ++i) {
			float theta1 = (2.0f * static_cast<float>(M_PI) * i) / segments;
			float theta2 = (2.0f * static_cast<float>(M_PI) * (i + 1)) / segments;

			Vector3 p1, p2;

			if (axis == 0) { // XY平面
				p1 = Vector3(radius * cosf(theta1), radius * sinf(theta1), 0.0f);
				p2 = Vector3(radius * cosf(theta2), radius * sinf(theta2), 0.0f);
			}
			else if (axis == 1) { // XZ平面
				p1 = Vector3(radius * cosf(theta1), 0.0f, radius * sinf(theta1));
				p2 = Vector3(radius * cosf(theta2), 0.0f, radius * sinf(theta2));
			}
			else { // YZ平面
				p1 = Vector3(0.0f, radius * cosf(theta1), radius * sinf(theta1));
				p2 = Vector3(0.0f, radius * cosf(theta2), radius * sinf(theta2));
			}

			// 中心座標をオフセット
			p1 += center;
			p2 += center;


			AddLine(p1, p2, lineColor);
		}
	}

	// XYZ 軸方向に intensity の強さを反映したラインを追加
	Vector3 intensityVectorX = { data.intensity, 0.0f, 0.0f };
	Vector3 intensityVectorY = { 0.0f, data.intensity, 0.0f };
	Vector3 intensityVectorZ = { 0.0f, 0.0f, data.intensity };

	Vector3 xStart = center - intensityVectorX;
	Vector3 xEnd = center + intensityVectorX;
	Vector3 yStart = center - intensityVectorY;
	Vector3 yEnd = center + intensityVectorY;
	Vector3 zStart = center - intensityVectorZ;
	Vector3 zEnd = center + intensityVectorZ;

	// 各軸のラインを描画
	AddLine(xStart, xEnd, lineColor); // X軸
	AddLine(yStart, yEnd, lineColor); // Y軸
	AddLine(zStart, zEnd, lineColor); // Z軸
};

/// <summary>
/// スポットライトデータを追加します。
/// </summary>
/// <param name="data">追加するスポットライトのデータ。</param>
void Engine::LineMeshData::AddLightLine(const SpotLightData& data) {
	constexpr int segments = 16; // スポットライトの円を構成するセグメント数
	const float distance = data.distance; // スポットライトの届く最大距離
	Vector3 center = data.position; // スポットライトの位置
	Vector4 lineColor = data.color; // ライトの色をそのまま使用

	// ライトの方向を正規化
	Vector3 dir = Normalize(data.direction);

	// スポットライトの最大開き角（cosAngleを使って角度を求める）
	float angle = acosf(data.cosAngle); // ラジアン
	float radius = tanf(angle) * distance; // スポット先端の半径

	// スポットライトの先端の位置（中心）
	Vector3 tip = center + dir * distance;

	// XYZ 軸に沿ったスポットの形状ラインを描画（スポット軸ライン）
	AddLine(center, tip, lineColor);

	// スポットライトの広がりを示す円を描画
	for (int i = 0; i < segments; ++i)
	{
		float theta1 = (2.0f * static_cast<float>(M_PI) * i) / segments;
		float theta2 = (2.0f * static_cast<float>(M_PI) * (i + 1)) / segments;

		// 円の頂点を求める（スポットの先端部分の円周上の点）
		Vector3 p1 = tip + CreateLine::GetPerpendicularVector(dir) * (radius * cosf(theta1)) + CreateLine::GetUpVector(dir) * (radius * sinf(theta1));
		Vector3 p2 = tip + CreateLine::GetPerpendicularVector(dir) * (radius * cosf(theta2)) + CreateLine::GetUpVector(dir) * (radius * sinf(theta2));

		// 円周のラインを描画
		AddLine(p1, p2, lineColor);

		// スポットライトの中心から先端の円周の点へのライン
		AddLine(center, p1, lineColor);
	}
};

/// <summary>
/// AABB状にライン加算
/// </summary>
/// <param name="aabb"></param>
/// <param name="pos"></param>
/// <param name="color"></param>
void Engine::LineMeshData::AddLineAABB(const AABB& aabb, const Vector3& pos, const Vector4& color) {
	// AABB の最小・最大範囲をワールド座標に適用
	Vector3 min = aabb.min_ + pos;
	Vector3 max = aabb.max_ + pos;

	// AABB の 8 頂点を計算
	Vector3 vertices[8] = {
		{min.x, min.y, min.z}, {max.x, min.y, min.z},
		{min.x, max.y, min.z}, {max.x, max.y, min.z},
		{min.x, min.y, max.z}, {max.x, min.y, max.z},
		{min.x, max.y, max.z}, {max.x, max.y, max.z},
	};

	// AABB のエッジ（ラインを構成するインデックス）
	int edges[12][2] = {
		{0, 1}, {1, 3}, {3, 2}, {2, 0}, // 前面
		{4, 5}, {5, 7}, {7, 6}, {6, 4}, // 背面
		{0, 4}, {1, 5}, {2, 6}, {3, 7}  // 前後を結ぶ
	};

	// 頂点とラインを追加
	for (int i = 0; i < 12; ++i)
	{
		int idx1 = edges[i][0];
		int idx2 = edges[i][1];


		AddLine(vertices[idx1], vertices[idx2], color);
	}
};

/// <summary>
/// OBB状にライン加算
/// </summary>
/// <param name="obb"></param>
/// <param name="color"></param>
void Engine::LineMeshData::AddLineOBB(const OBB& obb, const Vector4& color) {
	// Half extents along each local axis
	Vector3 halfSize = obb.size * 1.0f;

	// ローカル軸方向にサイズを掛けて、各方向ベクトルにスケールを適用
	Vector3 axes[3] = {
		obb.orientations[0] * halfSize.x,
		obb.orientations[1] * halfSize.y,
		obb.orientations[2] * halfSize.z
	};

	// 8頂点（各コーナー）を構築
	Vector3 corners[8];
	int i = 0;
	for (int dx = -1; dx <= 1; dx += 2) {
		for (int dy = -1; dy <= 1; dy += 2) {
			for (int dz = -1; dz <= 1; dz += 2) {
				corners[i++] = obb.center
					+ axes[0] * static_cast<float>(dx)
					+ axes[1] * static_cast<float>(dy)
					+ axes[2] * static_cast<float>(dz);
			}
		}
	}

	// エッジのペア（indicesでアクセス）
	const int edgePairs[12][2] = {
		{0, 1}, {1, 3}, {3, 2}, {2, 0}, // bottom face
		{4, 5}, {5, 7}, {7, 6}, {6, 4}, // top face
		{0, 4}, {1, 5}, {2, 6}, {3, 7}  // vertical edges
	};

	// ラインを追加
	for (int e = 0; e < 12; ++e) {
		AddLine(corners[edgePairs[e][0]], corners[edgePairs[e][1]], color);
	}
}

/// <summary>
/// 球状にライン加算
/// </summary>
/// <param name="sphere"></param>
/// <param name="color"></param>
/// <param name="segmentW"></param>
/// <param name="segmentH"></param>
void Engine::LineMeshData::AddLineSphere(const Sphere& sphere, const Vector4& color, int segmentW, int segmentH)
{
	float radius = sphere.radius;
	Vector3 center = sphere.center;
	float pi = static_cast<float>(std::numbers::pi * 2.0f);


	for (int h = 0; h <= segmentH; ++h)
	{
		float theta = static_cast<float>(h) / segmentH * pi; // 緯度角 (0 ~ π)

		for (int w = 0; w < segmentW; ++w)
		{
			float phi1 = static_cast<float>(w) / segmentW * pi; // 経度角 (0 ~ 2π)
			float phi2 = static_cast<float>(w + 1) / segmentW * pi;

			// 緯度線
			Vector3 p1 = {
				center.x + radius * sinf(theta) * cosf(phi1),
				center.y + radius * cosf(theta),
				center.z + radius * sinf(theta) * sinf(phi1)
			};
			Vector3 p2 = {
				center.x + radius * sinf(theta) * cosf(phi2),
				center.y + radius * cosf(theta),
				center.z + radius * sinf(theta) * sinf(phi2)
			};
			AddLine(p1, p2, color);

			if (h < segmentH)
			{
				float theta2 = static_cast<float>(h + 1) / segmentH * pi;

				// 経度線
				Vector3 p3 = {
					center.x + radius * sinf(theta2) * cosf(phi1),
					center.y + radius * cosf(theta2),
					center.z + radius * sinf(theta2) * sinf(phi1)
				};
				AddLine(p1, p3, color);
			}
		}
	}
}

/// <summary>
/// コーナー状にライン加算
/// </summary>
/// <param name="corner"></param>
/// <param name="pos"></param>
void Engine::LineMeshData::AddLineCorner(const CornerSegment& corner, const WorldTransform& pos)
{
	std::vector<Vector3> vertices;
	float angleStep = DirectX::XM_2PI / corner.segment; // 360° を segment 分割

	// 回転行列を作成
	Matrix4x4 rotationMatrix = MakeRotateXYZ(pos.rotate_);

	// 頂点を計算
	for (int i = 0; i < corner.segment; ++i)
	{
		float angle = i * angleStep; // 各頂点の角度

		// ローカル座標で円を作成
		Vector3 localVertex;
		localVertex.x = cos(angle) * corner.radius;
		localVertex.y = 0.0f;
		localVertex.z = sin(angle) * corner.radius;

		// 回転を適用
		Vector3 rotatedVertex = rotationMatrix.Transform(localVertex);

		// 平行移動を適用 (ワールド座標へ変換)
		Vector3 worldVertex = rotatedVertex + pos.translate_;
		vertices.push_back(worldVertex);
	}

	// 各辺をラインで描画
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白色のライン
	for (int i = 0; i < corner.segment; ++i)
	{
		Vector3 start = vertices[i];
		Vector3 end = vertices[(i + 1) % corner.segment]; // ループするように処理

		AddLine(start, end, color);
	}
}

/// <summary>
/// カプセル状にライン加算
/// </summary>
/// <param name="capsule"></param>
/// <param name="color"></param>
void Engine::LineMeshData::AddLineCapsule(const Capsule& capsule, const Vector4& color)
{
	const int segmentCount = 8; // 半球と円の分割数
	const Vector3& start = capsule.segment.origin;
	const Vector3& end = capsule.segment.end;
	const float radius = capsule.radius;

	// カプセルの軸
	Vector3 axis = end - start;
	if (axis.Length() < 0.0001f) return; // 長さゼロをチェック
	axis = axis.Normalize();

	// 垂直な平面を構成するベクトル
	Vector3 up = fabs(axis.y) > 0.99f ? Vector3(1.0f, 0.0f, 0.0f) : Vector3(0.0f, 1.0f, 0.0f);
	Vector3 side = axis.Cross(up).Normalize();
	Vector3 forward = axis.Cross(side).Normalize();

	float pi = static_cast<float>(M_PI);

	// シリンダー部分の描画と球間の接続
	for (int i = 0; i < segmentCount; ++i) {
		float angle1 = (2 * pi / static_cast<float>(segmentCount)) * static_cast<float>(i);
		float angle2 = (2 * pi / static_cast<float>(segmentCount)) * static_cast<float>(i + 1);

		Vector3 offset1 = side * cosf(angle1) * radius + forward * sinf(angle1) * radius;
		Vector3 offset2 = side * cosf(angle2) * radius + forward * sinf(angle2) * radius;

		Vector3 off1S = start + offset1;
		Vector3 off1E = end + offset1;
		Vector3 off2S = start + offset2;
		Vector3 off2E = end + offset2;

		// シリンダーの縁を描画
		AddLine(off1S, off1E, color);
		AddLine(off2S, off2E, color);

		// 球と球の間に線を引く
		AddLine(start + offset1, start + offset2, color);
		AddLine(end + offset1, end + offset2, color);
	}

	// 半球の描画
	for (int i = 0; i < segmentCount; ++i) {
		float angle1 = (pi / static_cast<float>(segmentCount)) * static_cast<float>(i);
		float angle2 = (pi / static_cast<float>(segmentCount)) * static_cast<float>(i + 1);

		for (int j = 0; j < segmentCount; ++j) {
			float horizontalAngle1 = (2 * pi / static_cast<float>(segmentCount)) * static_cast<float>(j);
			float horizontalAngle2 = (2 * pi / static_cast<float>(segmentCount)) * static_cast<float>(j + 1);

			Vector3 offset1 = side * cosf(horizontalAngle1) * sinf(angle1) * radius + forward * sinf(horizontalAngle1) * sinf(angle1) * radius + axis * cosf(angle1) * radius;
			Vector3 offset2 = side * cosf(horizontalAngle2) * sinf(angle1) * radius + forward * sinf(horizontalAngle2) * sinf(angle1) * radius + axis * cosf(angle1) * radius;
			Vector3 offset3 = side * cosf(horizontalAngle1) * sinf(angle2) * radius + forward * sinf(horizontalAngle1) * sinf(angle2) * radius + axis * cosf(angle2) * radius;

			AddLine(start + offset1, start + offset2, color);
			AddLine(start + offset1, start + offset3, color);

			AddLine(end - offset1, end - offset2, color);
			AddLine(end - offset1, end - offset3, color);
		}
	}
}

/// <summary>
/// スプライン状にライン加算
/// </summary>
/// <param name="controlPoints"></param>
/// <param name="pos"></param>
/// <param name="color"></param>
void Engine::LineMeshData::AddSpline(const std::vector<Vector3>& controlPoints, const WorldTransform& pos, const Vector4& color)
{
	int SPLIT = static_cast<int>(4 * controlPoints.size());

	Vector3 splineStr{};
	Vector3 splineEnd{};
	//ライン
	for (int index = 0; index < SPLIT; index++) {
		float t0 = index / float(SPLIT);
		float t1 = (index + 1) / float(SPLIT);

		splineStr = CatmullRom(controlPoints, t0) + pos.worldMat_.GetWorldPosition();
		splineEnd = CatmullRom(controlPoints, t1) + pos.worldMat_.GetWorldPosition();


		AddLine(splineStr, splineEnd, color);
	}
};

/// <summary>
/// スプライン状にライン加算
/// </summary>
/// <param name="controlPoints"></param>
/// <param name="pos"></param>
/// <param name="color"></param>
void Engine::LineMeshData::AddSpline(const std::vector<Vector3>& controlPoints, const Vector3& pos, const Vector4& color)
{
	int SPLIT = static_cast<int>(4 * controlPoints.size());

	Vector3 splineStr{};
	Vector3 splineEnd{};
	//ライン
	for (int index = 0; index < SPLIT; index++) {
		float t0 = index / float(SPLIT);
		float t1 = (index + 1) / float(SPLIT);

		splineStr = CatmullRom(controlPoints, t0) + pos;
		splineEnd = CatmullRom(controlPoints, t1) + pos;


		AddLine(splineStr, splineEnd, color);
	}

}

/// <summary>
/// 三角形上にライン加算
/// </summary>
/// <param name="triangle"></param>
/// <param name="pos"></param>
void Engine::LineMeshData::AddLineTriangle(const Triangle& triangle, const WorldTransform& pos)
{
	// 三角形の3つの頂点
	Vector3 p0 = triangle.vertices[0] + pos.worldMat_.GetWorldPosition();
	Vector3 p1 = triangle.vertices[1] + pos.worldMat_.GetWorldPosition();
	Vector3 p2 = triangle.vertices[2] + pos.worldMat_.GetWorldPosition();

	AddLine(p0, p1, Vector4{ 1,1,1,1 });
	AddLine(p1, p2, Vector4{ 1,1,1,1 });
	AddLine(p2, p0, Vector4{ 1,1,1,1 });
}

/// <summary>
/// オクツリー状にライン加算
/// </summary>
/// <param name="node"></param>
void Engine::LineMeshData::AddOctree(const OctreeNode& node)
{
	// AABB を描画

	AddLineAABB(node.bounds, { 0,0,0 });

	// 子ノードがある場合は再帰的に描画
	for (OctreeNode* child : node.children) {
		if (child) {  // nullptr チェック
			AddOctree(*child);
		}
	}
}

void Engine::LineMeshData::AddGrid(float xRange, float zRange, float interval, const Vector4& color)
{
	if (interval <= 0.0f) return;

	// X軸の中心線（赤）
	AddLine(Vector3(-xRange, 0.0f, 0.0f), Vector3(xRange, 0.0f, 0.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	// Z軸の中心線（赤）
	AddLine(Vector3(0.0f, 0.0f, -zRange), Vector3(0.0f, 0.0f, zRange), Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	// X軸方向のグリッド線
	for (float x = -xRange; x <= xRange; x += interval)
	{
		if (x == 0.0f) continue; // 中心線は既に描画済み
		Vector4 lineColor = (fmodf(fabs(x), 10.0f) < 0.01f) ? Vector4(0.0f, 0.0f, 1.0f, 1.0f) : color;
		AddLine(Vector3(x, 0.0f, -zRange), Vector3(x, 0.0f, zRange), lineColor);
	}

	// Z軸方向のグリッド線
	for (float z = -zRange; z <= zRange; z += interval)
	{
		if (z == 0.0f) continue; // 中心線は既に描画済み
		Vector4 lineColor = (fmodf(fabs(z), 10.0f) < 0.01f) ? Vector4(0.0f, 0.0f, 1.0f, 1.0f) : color;
		AddLine(Vector3(-xRange, 0.0f, z), Vector3(xRange, 0.0f, z), lineColor);
	}
}









// ライン作成
void Engine::CreateLine::Line(LineMeshData& meshData, const LineData& lineData) {

	// 始点と終点と色を設定
	meshData.GetMesh()->verticesline.push_back({{lineData.start_.x, lineData.start_.y, lineData.start_.z, 1.0f}, lineData.color_});
	meshData.GetMesh()->verticesline.push_back({{lineData.end_.x, lineData.end_.y, lineData.end_.z, 1.0f}, lineData.color_});

	meshData.GetMesh()->indices.push_back(meshData.GetCount());
	meshData.GetMesh()->indices.push_back(meshData.GetCount() + 1);

	// ライン使用量加算
	meshData.AddCount(2);
};