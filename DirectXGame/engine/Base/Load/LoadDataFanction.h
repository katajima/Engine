#pragma once
#include "LevelData.h"
#include <json.hpp>
#include <vector>
#include <map>


namespace LoadDataFanc 
{
	// ファイル読みこみ
	nlohmann::json FileData(const std::string fullpath);
	// モデル位置の設定
	void ModelTransfom(nlohmann::json& object, LevelData* levelData);
	// 出現位置設定
	void SpawwnPoint(nlohmann::json& object, LevelData* levelData);
	// カメラ位置
	void CameraTransform(nlohmann::json& object, LevelData* levelData);
	// ライト位置
	void LightTransform(nlohmann::json& object, LevelData* levelData);
}

