#include "TrailEffect.h"
#include "DirectXGame/engine/3d/Object/Object3d.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

void Engine::TrailEffect::Initialize(EffectManager* effectManager ,const std::string& tex,float maxtime  ,const Color color)
{
	// エフェクト管理クラス
	effectManager_ = effectManager;

	// メッシュ生成
	mesh = std::make_unique<ModelMesh>();
	mesh->vertices.push_back({ 0,0,0 });
	mesh->indices.push_back(1);
	mesh->Initialize(effectManager_->GetDxCommon());
	mesh->indices.clear();
	mesh->vertices.clear();
	mesh->maxTime = maxtime;

	// 生存時間設定
	timer = maxtime;

	// マテリアル初期化
	material = std::make_unique<Material>();
	material->Initialize(effectManager_->GetDxCommon());
	material->tex_.diffuseFilePath = tex;
	material->color = color;

	// トランスフォーム初期化
	transfomation = std::make_unique<Transfomation>();
	transfomation->Initialize(effectManager_->GetDxCommon());


	parentTransform_.Identity();
	mat_.Identity();
}

void Engine::TrailEffect::Update()
{
	material->GPUData();
	worldtransformTstr_.Update();
	worldtransformTend_.Update();

	Vector3 str = worldtransformTstr_.worldMat_.GetWorldPosition();
	Vector3 end = worldtransformTend_.worldMat_.GetWorldPosition();
	Vector3 strPre = worldtransformTstr_.worldPreMat_.GetWorldPosition();
	Vector3 endPre = worldtransformTend_.worldPreMat_.GetWorldPosition();




	Vector3 leftTop = { str.x , str.y , str.z };
	Vector3 leftBottom = { end.x , end.y , end.z };
	Vector3 rightTop = { strPre.x , strPre.y , strPre.z };
	Vector3 rightBottom = { endPre.x , endPre.y , endPre.z };

	//　出すなら二枚の三角ポリゴンを 
	if (flag_) {
		mesh->vertices.push_back({ .position = {leftTop.x, leftTop.y, leftTop.z, 1.0f}, .texcoord = {0.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });	// 左上
		mesh->vertices.push_back({ .position = {rightTop.x, rightTop.y, rightTop.z, 1.0f}, .texcoord = {1.0f, 0.0f}, .normal = {0.0f, 0.0f, 1.0f} });	// 右上
		mesh->vertices.push_back({ .position = {leftBottom.x, leftBottom.y, leftBottom.z, 1.0f} ,.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f } });	// 左下
		mesh->vertices.push_back({ .position = {leftBottom.x, leftBottom.y, leftBottom.z, 1.0f} ,.texcoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f } });	// 左下
		mesh->vertices.push_back({ .position = {rightTop.x, rightTop.y, rightTop.z, 1.0f} ,.texcoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f } });	// 右上
		mesh->vertices.push_back({ .position = {rightBottom.x, rightBottom.y, rightBottom.z, 1.0f} ,.texcoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f } });	// 右下

		// タイマーの初期化
		for (int i = 0; i < 6; ++i) {
			mesh->verticesTimer.push_back({ 0.0f });
		}
	}


	// タイマーの更新
	for (size_t i = 0; i < mesh->verticesTimer.size(); ++i) {
		mesh->verticesTimer[i] += MyGame::GameTime(); // 例としてフレーム時間を加算 (60FPSの想定)
	}


	// 時間が経過した頂点を削除
	while (!mesh->verticesTimer.empty() && mesh->verticesTimer.front() >= mesh->maxTime) {
		mesh->vertices.erase(mesh->vertices.begin());
		mesh->verticesTimer.erase(mesh->verticesTimer.begin());
	}




	// 頂点バッファビューを更新
	mesh->UpdateVertexBuffer();




	parentTransform_ = MakeAffineMatrix({ 1,1,1 }, Vector3{ 0,0,0 }, { 0,0,0 });

	// 位置データ
	transfomation->Update(camera_, parentTransform_);
}

void Engine::TrailEffect::Draw()
{
	// 頂点があるなら
	if (mesh->vertices.size() != 0) {
		
		// 描画前準備
		effectManager_->GetTrailEffectCommon()->DrawCommonSetting();
		
		// 位置
		transfomation->GetCommandList(1);

		// マテリアル
		material->GetCommandListMaterial(0);

		// テクスチャ
		material->GetCommandListTexture(2, 7, 8);

		// メッシュ
		mesh->GetCommandList();

		// 描画コマンドの修正：インスタンス数の代わりにインデックス数を使用
		effectManager_->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(mesh->vertices.size()), 1, 0, 0);

	}
}


