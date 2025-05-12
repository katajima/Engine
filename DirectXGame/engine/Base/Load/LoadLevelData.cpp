#include "LoadLevelData.h"
#include "DirectXGame/engine/3d/Model/ModelManager.h"


void LoadLevelData::Initialize(Entity3DManager* entity3DManager, ModelManager* modelManager, Camera* camera,const std::string extensionName, const std::string fileName)
{
	const std::string kDefaultBaseDirectory = "resources/";
	const std::string kFileName = fileName;
	const std::string kExtension = extensionName;
	// 凍結してフルパスを得る 
	const std::string fullpath = kDefaultBaseDirectory + kFileName + kExtension;
	// ファイルストリーム 
	std::ifstream file; // ファイルを開く
	file.open(fullpath);
	// ファイルオープン失敗チェック 
	if (file.fail()) { assert(0); }
	// JSON文字列から解凍したデータ 
	nlohmann::json deserialized;
	// 解凍 
	file >> deserialized;
	// 正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());
	// "name"を文字列として取得 
	std::string name = deserialized["name"].get<std::string>();
	// 正しいレベルデータファイルかチェック 
	assert(name.compare("scene") == 0);
	// レベルデータ格納用インスタンスを生成 
	LevelData* levelData = new LevelData();
	const auto& models = modelManager->GetModel();
	// "objects"の全オブジェクトを走査 
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));
		// 種別を取得 
		std::string type = object["type"].get<std::string>();
		if (type.compare("MESH") == 0) {
			// 要素追加 
			levelData->objects.emplace_back(LevelData::ObjectData{});
			//今追加した要素の参照を得る 
			LevelData::ObjectData& objectData = levelData->objects.back();
			if (object.contains("name")) {
				// ファイル名 
				objectData.fileName = object["name"];
			}
			// トランスフォームのパラメータ読み込み 
			nlohmann::json& transform = object["transform"];
			// 平行移動 
			objectData.position.x = (float)transform["translation"][0];
			objectData.position.y = (float)transform["translation"][1];
			objectData.position.z = (float)transform["translation"][2];
			// 回転 
			objectData.rotation.x = (float)transform["rotation"][0];
			objectData.rotation.y = (float)transform["rotation"][1];
			objectData.rotation.z = (float)transform["rotation"][2];
			// スケーリング 
			objectData.scale.x = (float)transform["scaling"][0];
			objectData.scale.y = (float)transform["scaling"][1];
			objectData.scale.z = (float)transform["scaling"][2];
			// 再帰関数にまとめ、再帰呼出で枝を走査する 
			if (object.contains("children")) {}
		} for (auto& objectData : levelData->objects) {
			// ファイル名から登録済みモデルを検索
			Model* model = nullptr;
			auto it = models.find(objectData.fileName + ".obj");
			if (it != models.end()) { model = it->second.get(); }
			// モデルを指定して3Dオブジェクトを生成 
			std::unique_ptr<Object3d> newObject = std::make_unique<Object3d>();
			newObject->Initialize(entity3DManager);
			newObject->SetModel(model);
			newObject->SetCamera(camera);
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
}


void LoadLevelData::Update()
{

	for (auto& object : objects) {
		object->Update();
	}

}

void LoadLevelData::Draw3D()
{
	for (auto& object : objects) {
		object->Draw(Object3d::ObjectRasterizerType::NoUvInterpolation_MODE_SOLID_BACK);
	}
}
