#include "Primitive2D.h"
#include "SpriteCommon.h"

void Primitive2D::Initialize(ShapeType type, const Color color)
{
	type_ = type;
	spriteCommon_ = SpriteCommon::GetInstance();


	mesh = std::make_unique<Mesh>();
	switch (type_)
	{
	case Primitive2D::ShapeType::Cube:
		CreateCube({100,100});
		break;
	case Primitive2D::ShapeType::Triangle:
		CreateTriangle({50.0f,-50.0f},{0,50.0f},{100.0f,50.0f});
		break;
	case Primitive2D::ShapeType::Circle:
		CreateCircle(100,32);
		break;
	case Primitive2D::ShapeType::Star:
		CreateStar(50,100,5);
		break;
	case Primitive2D::ShapeType::Ring:
		CreateRing(50, 100, 5);
		break;
	default:
		break;
	}

	// メッシュ	
	mesh->Initialize(spriteCommon_->GetDxCommon());

	// マテリアル
	material = std::make_unique<Material>();
	material->Initialize(spriteCommon_->GetDxCommon());
	material->tex_.diffuseFilePath = "resources/Texture/Image.png";
	material->color = color;

	// トランスフォーム
	transfomation = std::make_unique<Transfomation>();
	transfomation->Initialize(spriteCommon_->GetDxCommon());



	//transform変数を作る
	transform = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

}

void Primitive2D::Update()
{
	transform.scale = { scale.x,scale.y,1.0f };
	transform.rotate = { 0.0f,0.0f,rotation };
	transform.translate = { position.x,position.y,0.0f };




	//transform変数を作る
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	////透視射影行列
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::GetClientWidth()), float(WinApp::GetClientHeight()), 0.0f, 100.0f);
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

	// トランスフォーム
	transfomation->UpdateSprite(worldViewProjectionMatrix);

	// マテリアル
	material->GPUData();
}

void Primitive2D::Draw()
{
	spriteCommon_->DrawCommonSetting(SpriteCommon::PSOType::UvInterpolation_MODE_SOLID);

	material->GetCommandListMaterial(0);

	material->GetCommandListTexture(2, 2, 2);

	//トランスフォームMatrixResource
	transfomation->GetCommandList(1);

	mesh->GetCommandList();

	spriteCommon_->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(UINT(mesh->indices.size()), 1, 0, 0, 0);
}

#pragma region Create

void Primitive2D::CreateTriangle(Vector2 p0, Vector2 p1, Vector2 p2)
{
	// 既存の頂点とインデックスをクリア
	mesh->vertices.clear();
	mesh->indices.clear();

	// 法線を +Z 方向に変更
	mesh->vertices.push_back({ .position = {p0.x, p0.y, 0.0f, 1.0f}, .texcoord = {0.5f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 上
	mesh->vertices.push_back({ .position = {p1.x, p1.y, 0.0f, 1.0f}, .texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 右下
	mesh->vertices.push_back({ .position = {p2.x, p2.y, 0.0f, 1.0f}, .texcoord = {1.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 左下

	// **反時計回り (CCW) に変更**
	mesh->indices.push_back(0);
	mesh->indices.push_back(2);
	mesh->indices.push_back(1);
}

void Primitive2D::CreateCircle(float radius, int segments)
{
	// 既存の頂点とインデックスをクリア
	mesh->vertices.clear();
	mesh->indices.clear();

	// 中心の頂点
	mesh->vertices.push_back({
		.position = {0.0f, 0.0f, 0.0f, 1.0f},  // 中心座標
		.texcoord = {0.5f, 0.5f},              // UV中心
		.normal = {0.0f, 0.0f, -1.0f}          // 法線の向きを反転
		});

	// 円周の頂点
	for (int i = 0; i < segments; ++i) { // `i <= segments` だと最後に重複するため `< segments`
		float theta = 2.0f * float(M_PI) * i / segments; // 角度を計算
		float x = radius * cos(theta);
		float y = radius * sin(theta);

		mesh->vertices.push_back({
			.position = {x, y, 0.0f, 1.0f},       // 円周の座標
			.texcoord = {(x / (2.0f * radius) + 0.5f), (y / (2.0f * radius) + 0.5f)}, // UV座標
			.normal = {0.0f, 0.0f, -1.0f}         // 法線の向きを反転
			});
	}

	// インデックスの設定 (CW: 時計回り)
	for (int i = 1; i < segments; ++i) {
		mesh->indices.push_back(0);       // 中心点
		mesh->indices.push_back(i);       // 現在の頂点
		mesh->indices.push_back(i + 1);   // 次の頂点
	}
	// 最後の三角形 (円の閉じる部分)
	mesh->indices.push_back(0);
	mesh->indices.push_back(segments);
	mesh->indices.push_back(1); // 最初の円周頂点とつなげる
}

void Primitive2D::CreateRing(float innerRadius, float outerRadius, int segments)
{
	if (innerRadius > outerRadius || segments < 3) {
		throw std::invalid_argument("Invalid parameters: innerRadius must be smaller than outerRadius, and segments must be >= 3.");
	}

	// 既存の頂点とインデックスをクリア
	mesh->vertices.clear();
	mesh->indices.clear();

	// 頂点生成
	for (int i = 0; i <= segments; ++i) {
		float theta = 2.0f * float(M_PI) * i / segments; // 角度を計算
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);

		// 外側の頂点
		mesh->vertices.push_back({
			.position = {outerRadius * cosTheta, outerRadius * sinTheta, 0.0f, 1.0f},  // 外側の座標
			.texcoord = {cosTheta * 0.5f + 0.5f, sinTheta * 0.5f + 0.5f},             // UV座標 (修正)
			.normal = {0.0f, 0.0f, 1.0f}                                             // 法線 (そのまま)
			});

		// 内側の頂点
		mesh->vertices.push_back({
			.position = {innerRadius * cosTheta, innerRadius * sinTheta, 0.0f, 1.0f}, // 内側の座標
			.texcoord = {cosTheta * 0.25f + 0.5f, sinTheta * 0.25f + 0.5f},           // UV座標 (修正)
			.normal = {0.0f, 0.0f, 1.0f}                                             // 法線 (そのまま)
			});
	}

	// インデックス生成 (CCW: 反時計回り)
	for (int i = 0; i < segments; ++i) {
		int outer1 = i * 2;
		int inner1 = outer1 + 1;
		int outer2 = outer1 + 2;
		int inner2 = inner1 + 2;

		// 一つ目の三角形 (CCWに修正)
		mesh->indices.push_back(outer1);
		mesh->indices.push_back(outer2);
		mesh->indices.push_back(inner1);

		// 二つ目の三角形 (CCWに修正)
		mesh->indices.push_back(inner1);
		mesh->indices.push_back(outer2);
		mesh->indices.push_back(inner2);
	}

}

void Primitive2D::CreateStar(float innerRadius, float outerRadius, int segments)
{
	if (segments < 3) {
		throw std::invalid_argument("Segments must be >= 3.");
	}

	// 既存の頂点とインデックスをクリア
	mesh->vertices.clear();
	mesh->indices.clear();

	// 中心の頂点を追加
	mesh->vertices.push_back({
		.position = {0.0f, 0.0f, 0.0f, 1.0f}, // 中心の頂点座標
		.texcoord = {0.5f, 0.5f},             // 中心のUV座標
		.normal = {0.0f, 0.0f, 1.0f}          // 法線を正方向に
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
			.position = {x, y, 0.0f, 1.0f}, // 頂点座標
			.texcoord = {x / (2.0f * outerRadius) + 0.5f, y / (2.0f * outerRadius) + 0.5f}, // UV座標
			.normal = {0.0f, 0.0f, 1.0f} // 法線を正方向に
			});
	}

	// インデックス生成 (CCW: 反時計回り)
	for (int i = 1; i <= segments * 2; ++i) {
		mesh->indices.push_back(0); // 中心点
		mesh->indices.push_back(i); // 現在の頂点
		mesh->indices.push_back((i % (segments * 2)) + 1); // 次の頂点（ループ処理）
	}
}

void Primitive2D::CreateCube(Vector2 size)
{
	// 既存の頂点とインデックスをクリア
	mesh->vertices.clear();
	mesh->indices.clear();

	Vector2 halfSize = size * 0.5f;

	// +Z 方向を向く四角形
	mesh->vertices.push_back({ .position = {-halfSize.x,  halfSize.y, 0.0f, 1.0f}, .texcoord = {0.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 左上
	mesh->vertices.push_back({ .position = { halfSize.x,  halfSize.y, 0.0f, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 右上
	mesh->vertices.push_back({ .position = {-halfSize.x, -halfSize.y, 0.0f, 1.0f}, .texcoord = {0.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 左下
	mesh->vertices.push_back({ .position = { halfSize.x, -halfSize.y, 0.0f, 1.0f}, .texcoord = {1.0f, 1.0f}, .normal = {0.0f, 0.0f, 1.0f} }); // 右下

	// **時計回り (CW) に修正**
	mesh->indices.push_back(0);
	mesh->indices.push_back(2);
	mesh->indices.push_back(1);
	mesh->indices.push_back(2);
	mesh->indices.push_back(3);
	mesh->indices.push_back(1);
}


// セット
void Primitive2D::SetParametar(float innerRadius, float outerRadius, int segments)
{
	switch (type_)
	{
	case Primitive2D::ShapeType::Star:
		CreateStar(innerRadius, outerRadius, segments);
		mesh->UpdateVertexBuffer();
		mesh->UpdateIndexBuffer();
		break;
	case Primitive2D::ShapeType::Ring:
		CreateRing(innerRadius, outerRadius, segments);
		mesh->UpdateVertexBuffer();
		mesh->UpdateIndexBuffer();
		break;
	default:
		break;
	}
}

void Primitive2D::SetParametar(float radius, int segments)
{
	switch (type_)
	{
	case Primitive2D::ShapeType::Circle:
		CreateCircle(radius, segments);
		mesh->UpdateVertexBuffer();
		mesh->UpdateIndexBuffer();
		break;
	default:
		break;
	}
}

void Primitive2D::SetParametar(Vector2 size)
{
	switch (type_)
	{
	case Primitive2D::ShapeType::Cube:
		CreateCube({ size.x,size.y });
		mesh->UpdateVertexBuffer();
		mesh->UpdateIndexBuffer();
		break;
	default:
		break;
	}
	
}

void Primitive2D::SetParametar(Vector2 p0, Vector2 p1, Vector2 p2)
{
	switch (type_)
	{
	case Primitive2D::ShapeType::Triangle:
		CreateTriangle(p0, p1, p2);
		mesh->UpdateVertexBuffer();
		mesh->UpdateIndexBuffer();
		break;
	default:
		break;
	}
}

#pragma endregion // 作成


