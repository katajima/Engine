#include "LoadDataFunction.h"
#include "DirectXGame/engine/3d/Model/ModelManager.h"


namespace {
	static inline float kLightIntensityConversion = 1000000.0f;
}

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
		if (object.contains("file_name")) {
			// ファイル名
			objectData.fileName = object["file_name"];
		}
		if (object.contains("collider")) {
			auto& collider = object["collider"];
			if (collider.contains("collider_size")) {
				auto& size = collider["collider_size"];
				objectData.size.x = size[0];
				objectData.size.y = size[2];
				objectData.size.z = size[1];
			}
			if (collider.contains("collider")) {
				objectData.type = collider["collider"];
			}
			if (collider.contains("collider_disabled")) {
				objectData.isCollider = collider["collider_disabled"];
			}
		}
		else {
			objectData.isCollider = false;
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

void LoadDataFanc::SpawwnPoint(nlohmann::json& object, LevelData* levelData)
{
	assert(object.contains("type"));
	// 種別を取得
	std::string type = object["type"].get<std::string>();
	if (type.compare("PlayerSpawn") == 0) {
		// 要素追加
		levelData->players.emplace_back(LevelData::PlayerSpawnData{});
		//今追加した要素の参照を得る
		LevelData::PlayerSpawnData& playerSpawnData = levelData->players.back();

		// トランスフォームのパラメータ読み込み
		nlohmann::json& transform = object["transform"];
		// 平行移動
		playerSpawnData.position.x = (float)transform["translation"][0];
		playerSpawnData.position.y = (float)transform["translation"][2];
		playerSpawnData.position.z = (float)transform["translation"][1];
		// 回転
		playerSpawnData.rotation.x = -(float)transform["rotation"][0];
		playerSpawnData.rotation.y = -(float)transform["rotation"][2];
		playerSpawnData.rotation.z = -(float)transform["rotation"][1];
	}
	else if (type.compare("EnemySpawn") == 0) {
		// 要素追加
		levelData->enemys.emplace_back(LevelData::EnemySpawnData{});
		levelData->counts.emplace_back(0);
		//今追加した要素の参照を得る
		LevelData::EnemySpawnData& enemySpawnData = levelData->enemys.back();
		int& countData = levelData->counts.back();
		if (object.contains("file_name")) {
			// ファイル名
			enemySpawnData.fileName = object["file_name"];
		}
		if (object.contains("emitSpawn")) {
			auto& emitSpawn = object["emitSpawn"];
			//enemySpawnData.crrentCount = 0;
			enemySpawnData.crrentTimer = 0;
			if (emitSpawn.contains("spawn_enable")) {
				enemySpawnData.isEnable = emitSpawn["spawn_enable"];
			}
			if (emitSpawn.contains("spawn_count")) {
				enemySpawnData.count = emitSpawn["spawn_count"];
			}
			if (emitSpawn.contains("spawn_count")) {
				enemySpawnData.timer = emitSpawn["spawn_timer"];
			}
		}

		// トランスフォームのパラメータ読み込み
		nlohmann::json& transform = object["transform"];
		// 平行移動
		enemySpawnData.position.x = (float)transform["translation"][0];
		enemySpawnData.position.y = (float)transform["translation"][2];
		enemySpawnData.position.z = (float)transform["translation"][1];
		// 回転
		enemySpawnData.rotation.x = -(float)transform["rotation"][0];
		enemySpawnData.rotation.y = -(float)transform["rotation"][2];
		enemySpawnData.rotation.z = -(float)transform["rotation"][1];
	}
}

void LoadDataFanc::CameraTransform(nlohmann::json& object, LevelData* levelData)
{
	assert(object.contains("type"));
	// 種別を取得
	std::string type = object["type"].get<std::string>();
	if (type.compare("CAMERA") == 0) {
		// 要素追加
		levelData->cameras.emplace_back(LevelData::CameraObject{});
		//今追加した要素の参照を得る
		LevelData::CameraObject& objectData = levelData->cameras.back();
		if (object.contains("file_name")) {
			// ファイル名
			objectData.fileName = object["file_name"];
		}
		// トランスフォームのパラメータ読み込み
		nlohmann::json& transform = object["transform"];
		// 平行移動
		objectData.position.x = (float)transform["translation"][0];
		objectData.position.y = (float)transform["translation"][2];
		objectData.position.z = (float)transform["translation"][1];
		// 回転
		objectData.rotation.x = -(float)(transform["rotation"][0] -90);
		objectData.rotation.y = -(float)(transform["rotation"][2]);
		objectData.rotation.z = (float)transform["rotation"][1];
		// スケーリング
		objectData.scale.x = 1.0f;
		objectData.scale.y = 1.0f;
		objectData.scale.z = 1.0f;
		// 再帰関数にまとめ、再帰呼出で枝を走査する
		//if (object.contains("children")) {}
	}
}

void LoadDataFanc::LightTransform(nlohmann::json& object, LevelData* levelData)
{
	assert(object.contains("type"));
	// 種別を取得
	std::string type = object["type"].get<std::string>();
	if (type.compare("LIGHT") == 0) {
		if (object.contains("light_data")) {
			auto& lightData = object["light_data"];
			if (lightData.contains("light_type")) {
				if (lightData["light_type"] == "POINT") {// ポイントライト
					levelData->pointDatas.emplace_back(PointLightData{});
					PointLightData& objectData = levelData->pointDatas.back();
					if (lightData.contains("light_enabled")) {
						objectData.isLight = static_cast<uint32_t>(lightData["light_enabled"]);
					}
					if (lightData.contains("light_radius")) {
						objectData.radius = lightData["light_radius"];
					}
					if (lightData.contains("light_intensity")) {
						objectData.intensity = lightData["light_intensity"];
					}
					if (lightData.contains("light_color")) {
						objectData.color.x = lightData["light_color"][0];
						objectData.color.y = lightData["light_color"][1];
						objectData.color.z = lightData["light_color"][2];
						objectData.color.w = 1.0f;
					}
					// トランスフォームのパラメータ読み込み
					nlohmann::json& transform = object["transform"];
					// 平行移動
					objectData.position.x = (float)transform["translation"][0];
					objectData.position.y = (float)transform["translation"][2];
					objectData.position.z = (float)transform["translation"][1];
				}
				else if (lightData["light_type"] == "DIRECTIONAL") { // 平行光源
					levelData->directionalDatas.emplace_back(DirectionalLightData{});
					DirectionalLightData& objectData = levelData->directionalDatas.back();
					if (lightData.contains("light_enabled")) {
						objectData.isLight = static_cast<uint32_t>(lightData["light_enabled"]);
					}
					if (lightData.contains("light_intensity")) {
						objectData.intensity = lightData["light_intensity"];
					}
					if (lightData.contains("light_color")) {
						objectData.color.x = lightData["light_color"][0];
						objectData.color.y = lightData["light_color"][1];
						objectData.color.z = lightData["light_color"][2];
						objectData.color.w = 1.0f;
					}
					// トランスフォームのパラメータ読み込み
					nlohmann::json& transform = object["transform"];
					// 平行移動
					objectData.direction.x = -(float)transform["rotation"][0];
					objectData.direction.y = -(float)transform["rotation"][2];
					objectData.direction.z = -(float)transform["rotation"][1];
					Normalize(objectData.direction);
				}
				else if (lightData["light_type"] == "SPOT") { // 平行光源
					levelData->spotDatas.emplace_back(SpotLightData{});
					SpotLightData& objectData = levelData->spotDatas.back();
					if (lightData.contains("light_enabled")) {
						objectData.isLight = static_cast<uint32_t>(lightData["light_enabled"]);
					}
					if (lightData.contains("light_radius")) {
						objectData.distance = lightData["light_radius"];
					}
					if (lightData.contains("light_intensity")) {
						objectData.intensity = lightData["light_intensity"];
					}
					if (lightData.contains("light_color")) {
						objectData.color.x = lightData["light_color"][0];
						objectData.color.y = lightData["light_color"][1];
						objectData.color.z = lightData["light_color"][2];
						objectData.color.w = 1.0f;
					}
					// トランスフォームのパラメータ読み込み
					nlohmann::json& transform = object["transform"];
					// 平行移動
					objectData.position.x = (float)transform["translation"][0];
					objectData.position.y = (float)transform["translation"][2];
					objectData.position.z = (float)transform["translation"][1];
					// 平行移動
					objectData.direction.x = -(float)transform["rotation"][0];
					objectData.direction.y = -(float)transform["rotation"][2];
					objectData.direction.z = -(float)transform["rotation"][1];
					Normalize(objectData.direction);
				}
			}
		}
		else if (object.contains("blender_light")) {
			auto& lightData = object["blender_light"];

			if (lightData["blender_light_type"] == "POINT") { // ポイントライト
				levelData->pointDatas.emplace_back(PointLightData{});
				PointLightData& objectData = levelData->pointDatas.back();
				if (lightData.contains("cutoff_distance")) {
					objectData.radius = lightData["cutoff_distance"];
				}
				if (lightData.contains("color")) {
					objectData.color.x = lightData["color"][0];
					objectData.color.y = lightData["color"][1];
					objectData.color.z = lightData["color"][2];
					objectData.color.w = 1.0f;
				}
				if (lightData.contains("energy")) {
					objectData.intensity = lightData["energy"] / kLightIntensityConversion;
				}
				objectData.isLight = true;
				// トランスフォームのパラメータ読み込み
				nlohmann::json& transform = object["transform"];
				// 平行移動
				objectData.position.x = (float)transform["translation"][0];
				objectData.position.y = (float)transform["translation"][2];
				objectData.position.z = (float)transform["translation"][1];
			}
			if (lightData["blender_light_type"] == "SUN") { // ディレクショナルライト
				levelData->directionalDatas.emplace_back(DirectionalLightData{});
				DirectionalLightData& objectData = levelData->directionalDatas.back();
				if (lightData.contains("color")) {
					objectData.color.x = lightData["color"][0];
					objectData.color.y = lightData["color"][1];
					objectData.color.z = lightData["color"][2];
					objectData.color.w = 1.0f;
				}
				if (lightData.contains("energy")) {
					objectData.intensity = lightData["energy"] / 1000000.0f;
				}

				objectData.isLight = true;
				// トランスフォームのパラメータ読み込み
				nlohmann::json& transform = object["transform"];

				// 平行移動
				objectData.direction.x = -(float)transform["rotation"][0];
				objectData.direction.y = -(float)(transform["rotation"][2] + 90);
				objectData.direction.z = -(float)transform["rotation"][1];
				Normalize(objectData.direction);
			}
			if (lightData["blender_light_type"] == "SPOT") { // スポットライト
				levelData->spotDatas.emplace_back(SpotLightData{});
				SpotLightData& objectData = levelData->spotDatas.back();
				if (lightData.contains("custom_distance")) {
					objectData.distance = lightData["custom_distance"];
				}
				if (lightData.contains("spot_size")) {
					objectData.cosAngle = Math::DegreesToRadians(lightData["spot_size"]);
				}
				if (lightData.contains("spot_blend")) {
					objectData.cosFalloffStart = lightData["spot_blend"];
				}
				if (lightData.contains("color")) {
					objectData.color.x = lightData["color"][0];
					objectData.color.y = lightData["color"][1];
					objectData.color.z = lightData["color"][2];
					objectData.color.w = 1.0f;
				}
				if (lightData.contains("energy")) {
					objectData.intensity = lightData["energy"] / 1000000.0f;
				}
				objectData.isLight = true;
				// トランスフォームのパラメータ読み込み
				nlohmann::json& transform = object["transform"];
				// 平行移動
				objectData.position.x = (float)transform["translation"][0];
				objectData.position.y = (float)transform["translation"][2];
				objectData.position.z = (float)transform["translation"][1];
				// 平行移動
				objectData.direction.x = -(float)transform["rotation"][0];
				objectData.direction.y = -(float)(transform["rotation"][2] + 90);
				objectData.direction.z = (float)transform["rotation"][1];
				Normalize(objectData.direction);
			}
		}
	}
}
