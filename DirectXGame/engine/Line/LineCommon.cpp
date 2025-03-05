#include "LineCommon.h"
#include "combaseapi.h"
#include "DirectXGame/engine/base/SrvManager.h"
#include "DirectXGame/engine/Material/Material.h"


LineCommon* LineCommon::instance = nullptr;


LineCommon* LineCommon::GetInstance()
{
	if (instance == nullptr) {
		instance = new LineCommon;
	}
	return instance;
}

void LineCommon::Finalize()
{
	delete instance;
	instance = nullptr;
}

void LineCommon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;


	psoManager_ = std::make_unique<PSOManager>();
	psoManager_->Initialize(dxCommon_);

	CreateGraphicsPipeline();

	


	// マテリアル
	materialResource = dxCommon_->CreateBufferResource(sizeof(Material));
	// 書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	//今回は赤を書き込んで見る //白
	*materialData = MaterialData({ 1.0f, 1.0f, 1.0f, 1.0f }); //RGBA



	viewResource = dxCommon_->CreateBufferResource(sizeof(Matrix4x4));
	viewResource->Map(0, nullptr, reinterpret_cast<void**>(&cameraWVP));

	*cameraWVP = MakeIdentity4x4();

	
	mesh_ = std::make_unique<Mesh>();
	mesh_->verticesline.push_back({ 0,0,0,0 });
	mesh_->verticesline.push_back({ 0,0,0,0 });
	mesh_->indices.push_back({ 0 });
	mesh_->indices.push_back({ 1 });
	mesh_->InitializeLine(dxCommon_);

	mesh_->verticesline.clear();
	mesh_->indices.clear();
}

void LineCommon::AddLine(Vector3 start, Vector3 end, Vector4 color)
{
	mesh_->verticesline.push_back({ { start.x, start.y, start.z, 1.0f }, color });
	mesh_->verticesline.push_back({ { end.x, end.y, end.z, 1.0f }, color });

	mesh_->indices.push_back(lineNum_);
	mesh_->indices.push_back(lineNum_ + 1);

	lineNum_ += 2;
}

void LineCommon::AddLightLine(PointLightData data)
{
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

			// 頂点を追加
			mesh_->verticesline.push_back({ { p1.x, p1.y, p1.z, 1.0f }, lineColor });
			mesh_->verticesline.push_back({ { p2.x, p2.y, p2.z, 1.0f }, lineColor });

			// インデックスを追加
			mesh_->indices.push_back(lineNum_);
			mesh_->indices.push_back(lineNum_ + 1);

			lineNum_ += 2;
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
}

void LineCommon::AddLightLine(SpotLightData data)
{
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
		Vector3 p1 = tip + GetPerpendicularVector(dir) * (radius * cosf(theta1)) + GetUpVector(dir) * (radius * sinf(theta1));
		Vector3 p2 = tip + GetPerpendicularVector(dir) * (radius * cosf(theta2)) + GetUpVector(dir) * (radius * sinf(theta2));

		// 円周のラインを描画
		AddLine(p1, p2, lineColor);

		// スポットライトの中心から先端の円周の点へのライン
		AddLine(center, p1, lineColor);
	}
}


void LineCommon::AddLineMesh(Mesh* mesh, const Matrix4x4& worldMat)
{



	for (size_t i = 0; i < mesh->verticesline.size(); i++) {
		// ワールド行列を適用して変換
		Vector4 pos = mesh->verticesline[i].position;
		Vector4 worldPos = Transforms(pos, worldMat);

		mesh_->verticesline.push_back({ worldPos.x, worldPos.y, worldPos.z, 1 });
	}

	Mesh::MeshLine(mesh->indices,mesh_->indices, lineNum_);


	// インデックスオフセットを更新
	lineNum_ += static_cast<uint32_t>(mesh_->indices.size());
}

void LineCommon::AddLineMesh(Mesh* mesh, const Matrix4x4& worldMat, std::vector<uint32_t> cachedLineIndices)
{


	for (size_t i = 0; i < mesh->verticesline.size(); i++) {
		// ワールド行列を適用して変換
		Vector4 pos = mesh->verticesline[i].position;
		Vector4 worldPos = Transforms(pos, worldMat);

		mesh_->verticesline.push_back({ worldPos.x, worldPos.y, worldPos.z, 1 });
	}

	for (size_t i = 0; i < cachedLineIndices.size(); i++) {
		mesh_->indices.push_back(cachedLineIndices[i + lineNum_]);
	}
	

	// インデックスオフセットを更新
	lineNum_ += static_cast<uint32_t>(mesh_->indices.size());
}

void LineCommon::AddLineAABB(AABB aabb, Vector3 pos)
{
	// AABB の最小・最大範囲をワールド座標に適用
	Vector3 min = aabb.min + pos;
	Vector3 max = aabb.max + pos;

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

	// ラインの色（固定値または動的変更可能）
	Vector4 lineColor = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白色（必要なら引数に追加）

	// 頂点とラインを追加
	for (int i = 0; i < 12; ++i)
	{
		int idx1 = edges[i][0];
		int idx2 = edges[i][1];

		// 頂点を追加
		mesh_->verticesline.push_back({ { vertices[idx1].x, vertices[idx1].y, vertices[idx1].z, 1.0f }, lineColor });
		mesh_->verticesline.push_back({ { vertices[idx2].x, vertices[idx2].y, vertices[idx2].z, 1.0f }, lineColor });

		// インデックスを追加
		mesh_->indices.push_back(lineNum_);
		mesh_->indices.push_back(lineNum_ + 1);

		lineNum_ += 2;
	}
}

void LineCommon::AddLineCorner(CornerSegment corner, WorldTransform pos)
{
	std::vector<Vector3> vertices;
	float angleStep = DirectX::XM_2PI / corner.segment; // 360° を segment 分割

	// 回転行列を作成
	Matrix4x4 rotationMatrix =  MakeRotateXYZ(pos.rotate_);

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

void LineCommon::AddSpline(std::vector<Vector3> controlPoints, WorldTransform pos)
{
	int SPLIT = static_cast<int>(4 * controlPoints.size());

	Vector3 splineStr{};
	Vector3 splineEnd{};
	//ライン
	for (int index = 0; index < SPLIT; index++) {
		float t0 = index / float(SPLIT);
		float t1 = (index + 1) / float(SPLIT);

		splineStr = CatmullRom(controlPoints, t0) + pos.translate_;
		splineEnd = CatmullRom(controlPoints, t1) + pos.translate_;


		AddLine(splineStr, splineEnd,{1,1,1,1});
	}

}



void LineCommon::AddGrid(float xRange, float zRange, float interval, Vector4 color)
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


void LineCommon::Update()
{
	mesh_->UpdateLineVertexBuffer();
	mesh_->UpdateIndexBuffer();

	if (camera_ && cameraWVP) {
		*cameraWVP = camera_->viewProjectionMatrix_;  // データをコピー
	}
}

void LineCommon::DrawCommonSetting()
{
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState.Get()); //PSOを設定

	//形状を設定。PSOに設定している物とはまた別。同じものを設定すると考えておけば良い
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
}

void LineCommon::Draw()
{
	DrawCommonSetting();

	auto commandList = dxCommon_->GetCommandList();

	// SRV (インスタンシングデータ) をルートパラメータ [0] に設定
	commandList->SetGraphicsRootConstantBufferView(1, materialResource->GetGPUVirtualAddress());

	// ビューデータ
	commandList->SetGraphicsRootConstantBufferView(0, viewResource->GetGPUVirtualAddress());


	mesh_->GetCommandList();

	if (mesh_->indices.size() != 0) {
		//commandList->DrawInstanced(UINT(mesh_->verticesline.size()), 1, 0, 0);
		commandList->DrawIndexedInstanced(UINT(mesh_->indices.size()), 1, 0, 0, 0);
	}
}

void LineCommon::LineClear()
{
	lineNum_ = 0;
	mesh_->Clear();
}

void LineCommon::CreateRootSignature()
{
	D3D12_ROOT_PARAMETER rootParameters[2] = {};  // 2つのパラメーターを使う

	// カメラデータ
	psoManager_->SetRootParameter(rootParameters[0],0,D3D12_SHADER_VISIBILITY_VERTEX,D3D12_ROOT_PARAMETER_TYPE_CBV);
	// マテリアルデータ (b0) をピクセルシェーダで使用する
	psoManager_->SetRootParameter(rootParameters[1],0,D3D12_SHADER_VISIBILITY_PIXEL,D3D12_ROOT_PARAMETER_TYPE_CBV);

	psoManager_->SetRootSignature(rootSignature, rootParameters, _countof(rootParameters),nullptr,0);
}


void LineCommon::CreateGraphicsPipeline()
{
	CreateRootSignature();


#pragma region BlendState

	// BlendState(ブレンドステート)の設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

#pragma endregion //BlendState(ブレンドステート)

	// RasterizerState(ラスタライザステート)の設定
	psoManager_->SetRasterizerDesc(D3D12_CULL_MODE_BACK, D3D12_FILL_MODE_SOLID);


	// インプットレイアウト
	psoManager_->AddInputElementDesc("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	psoManager_->AddInputElementDesc("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);


	psoManager_->shderFile_.vertex.filePach = L"resources/shaders/Line/Line.VS.hlsl";
	psoManager_->shderFile_.pixel.filePach = L"resources/shaders/Line/Line.PS.hlsl";


	//DepthStencilStateの設定を行う
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	// Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	// 書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	psoManager_->GraphicsPipelineState(rootSignature, graphicsPipelineState, blendDesc, depthStencilDesc,D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
}
