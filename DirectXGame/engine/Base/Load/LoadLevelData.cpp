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

	// レベルデータ格納用インスタンスを生成 
	LevelData* levelData = new LevelData();
	// "objects"の全オブジェクトを走査 
	for (nlohmann::json& object : deserialized["objects"]) {
		// JSONファイルからトランスフォーム情報をレベルデータに書き出す
		LoadDataFanc::ModelTransfom(object, levelData);
		// レベルデータよりオブジェクトを生成する
		CreateObject3d(levelData);
	}
}

void LoadLevelData::ReLoad()
{
	for (auto& object : objects) {
		object->IsDelete();
	}	
	//objects.clear();

	nlohmann::json deserialized = LoadDataFanc::FileData(kDefaultBaseDirectory + kFileName + kExtension);

	// レベルデータ格納用インスタンスを生成 
	LevelData* levelData = new LevelData();
	// "objects"の全オブジェクトを走査 
	for (nlohmann::json& object : deserialized["objects"]) {	
		// JSONファイルからトランスフォーム情報をレベルデータに書き出す
		LoadDataFanc::ModelTransfom(object, levelData);
		// レベルデータよりオブジェクトを生成する
		CreateObject3d(levelData);
	}
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
		std::unique_ptr<Object3d> newObject = std::make_unique<Object3d>();
		newObject->Initialize(entity3DManager_);
		newObject->SetModel(model);
		newObject->SetIsDraw(true);
		// 座標 
		newObject->worldtransform_.translate_ = objectData.position;
		// 回転角 
		newObject->worldtransform_.rotate_ = objectData.rotation;
		// 大きさ
		newObject->worldtransform_.scale_ = objectData.scale;
		// 配列に登録
		objects.push_back(std::move(newObject));
	}
}
