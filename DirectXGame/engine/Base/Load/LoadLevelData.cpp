#include "LoadLevelData.h"
#include "DirectXGame/engine/3d/Model/ModelManager.h"
#include "DirectXGame/engine/manager/entity3d/Entity3DManager.h"

#include "LoadDataFanction.h"

void LoadLevelData::Initialize(Entity3DManager* entity3DManager, ModelManager* modelManager, Camera* camera,const std::string extensionName, const std::string fileName)
{
	entity3DManager_ = entity3DManager;
	modelManager_ = modelManager;

	kFileName = fileName;
	kExtension = extensionName;
	
	// 凍結してフルパスを得る 
	nlohmann::json deserialized = LoadDataFanc::FileData(kDefaultBaseDirectory + kFileName + kExtension);

	levelData_ = new LevelData();
	// "objects"の全オブジェクトを走査 
	for (nlohmann::json& object : deserialized["objects"]) {
		// 有効無効フラグ
		if (object.contains("disabled")) {
			bool disabled = object["disabled"].get<int>();
			if (disabled) {
				// 配置しない
				continue;
			}
		}
		
		// JSONファイルからトランスフォーム情報をレベルデータに書き出す(オブジェクト)
		LoadDataFanc::ModelTransfom(object, levelData_);
		// JSONファイルからトランスフォーム情報をレベルデータに書き出す(出現位置)
		LoadDataFanc::SpawwnPoint(object, levelData_);
	}
	// レベルデータよりオブジェクトを生成する
	CreateObject3d(levelData_);

}

void LoadLevelData::ReLoad()
{
	// "LevelObject" タグを持つオブジェクトを全削除
	entity3DManager_->EraseObject3DByTag("LevelObject");
	levelData_->objects.clear();
	levelData_->players.clear();
	levelData_->enemys.clear();
	objects_.clear();

	nlohmann::json deserialized = LoadDataFanc::FileData(kDefaultBaseDirectory + kFileName + kExtension);

	// "objects"の全オブジェクトを走査 
	for (nlohmann::json& object : deserialized["objects"]) {
		// 有効無効フラグ
		if (object.contains("disabled")) {
			bool disabled = object["disabled"].get<int>();
			if (disabled) {
				// 配置しない
				continue;
			}
		}

		// JSONファイルからトランスフォーム情報をレベルデータに書き出す
		LoadDataFanc::ModelTransfom(object, levelData_);
		// JSONファイルからトランスフォーム情報をレベルデータに書き出す(出現位置)
		LoadDataFanc::SpawwnPoint(object, levelData_);
	}
	// レベルデータよりオブジェクトを生成する
	CreateObject3d(levelData_);
}


void LoadLevelData::Update()
{
#ifdef _DEBUG
	ImGui::Begin("sceneData");
	if (ImGui::Button("ReLoad")) {
		ReLoad();
	}
	ImGui::End();
#endif // _DEBUG

}

void LoadLevelData::Draw3D()
{
	
}

void LoadLevelData::CreateObject3d(LevelData* levelData)
{
	const auto& models = modelManager_->GetModel();


	for (auto& objectData : levelData->objects) {
		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;

		// obj
		if (model == nullptr) {
			auto it = models.find(objectData.fileName + ".obj");
			if (it != models.end()) { model = it->second.get(); }
		}

		// gltf
		if (model == nullptr) {
			auto it = models.find(objectData.fileName + ".gltf");
			if (it != models.end()) { model = it->second.get(); }
		}

		// モデルを指定して3Dオブジェクトを生成 
		Object3d* newObject = entity3DManager_->CreateObject3D(objectData.fileName,ObjectModelType::kNormal,{},{});
		//newObject->Initialize(entity3DManager_);
		newObject->SetModel(model);
		newObject->SetIsDraw(true);
		newObject->SetNameTag("LevelObject");
		if (objectData.isCollider) {
			newObject->InitColliderComponent();

			// SphereColliderを追加
			auto aabb = std::make_unique<AABBCollider>();
			aabb->tag = CollisionTag::Wall;
			//sphere->layer = CollisionLayer::None;
			aabb->collisionMask = 0xFFFFFFFF;
			//aabb->radius = objectData.size.x;
			aabb->aabb.min_ = -((objectData.size) / 2)* objectData.scale;
			aabb->aabb.max_ = ((objectData.size) / 2)* objectData.scale;
			aabb->isStatic = true;
			newObject->GetColliderComponent()->AddCollider(std::move(aabb));
		}
		// 座標 
		newObject->GetWorldTransform().translate_ = objectData.position;
		// 回転角 
		newObject->GetWorldTransform().rotate_ = objectData.rotation;
		// 大きさ
		newObject->GetWorldTransform().scale_ = objectData.scale;

		objects_.push_back(newObject);
	}
}


