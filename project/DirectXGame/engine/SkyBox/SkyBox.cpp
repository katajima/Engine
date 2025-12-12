#include "SkyBox.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"

void Engine::SkyBox::Initialize(Entity3DManager* entity3DManager, std::string txtueName)
{
	entity3DManager_ = entity3DManager;


	// マテリアル初期化
	material = std::make_unique<Material>();
	material->Initialize(entity3DManager_->GetSkyBoxCommon()->GetDxCommon());
	material->tex_.diffuseFilePath = txtueName;	// テクスチャ名設定
	material->LoadTex();			// テクスチャ読み込み
	material->color = { 1,1,1,1 };	// 色
	material->enableLighting_ = false;	// ライティングしない



	mesh_ = std::make_unique<SkyBoxMesh>();

	Vector3 size = Vector3{ 10,10,10 } *100;

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

	// 各面のインデックスオフセット
	int vertexOffset = 0;

	for (int i = 0; i < 6; ++i) { // 6面
		// 頂点データを追加
		for (int j = 0; j < 4; ++j) { // 各面の4頂点
			mesh_->verticesskyBox.push_back({
				positions[i][j],      // 座標
				});
		}

		// インデックスデータを追加 (2つの三角形)
		mesh_->indices.push_back(vertexOffset + 0);
		mesh_->indices.push_back(vertexOffset + 2);
		mesh_->indices.push_back(vertexOffset + 1);

		mesh_->indices.push_back(vertexOffset + 2);
		mesh_->indices.push_back(vertexOffset + 3);
		mesh_->indices.push_back(vertexOffset + 1);

		vertexOffset += 4; // 次の面に移動
	}

	mesh_->Initialize(entity3DManager_->GetSkyBoxCommon()->GetDxCommon());
}

void Engine::SkyBox::Update()
{

}

void Engine::SkyBox::Draw()
{
	mesh_->GetCommandList();

	material->GetCommandListMaterial(0);

	material->GetCommandListTexture(2, 7, 8);

	// 描画コマンドの修正：インスタンス数の代わりにインデックス数を使用
	entity3DManager_->GetSkyBoxCommon()->GetDxCommon()->GetCommandList()->DrawIndexedInstanced(UINT(mesh_->indices.size()), 1, 0, 0, 0);
}
