#include "LoadDataFanction.h"
#include "DirectXGame/engine/3d/Model/ModelManager.h"

nlohmann::json LoadDataFanc::FileData(const std::string fullpath)
{
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

	return deserialized;
}

void LoadDataFanc::ModelTransfom(nlohmann::json& object, LevelData* levelData)
{
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
		objectData.position.y = (float)transform["translation"][2];
		objectData.position.z = (float)transform["translation"][1];
		// 回転 
		objectData.rotation.x = -(float)transform["rotation"][0];
		objectData.rotation.y = -(float)transform["rotation"][2];
		objectData.rotation.z = -(float)transform["rotation"][1];
		// スケーリング 
		objectData.scale.x = (float)transform["scaling"][0];
		objectData.scale.y = (float)transform["scaling"][2];
		objectData.scale.z = (float)transform["scaling"][1];
		// 再帰関数にまとめ、再帰呼出で枝を走査する 
		if (object.contains("children")) {}
	}
}
