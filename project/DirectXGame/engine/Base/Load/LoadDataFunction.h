#pragma once
#include "LevelData.h"
#include <json.hpp>
#include <vector>
#include <map>


namespace LoadDataFanc
{
	/// <summary>
	/// ファイル読みこみ
	/// </summary>
	nlohmann::json FileData(const std::string fullpath);
	/// <summary>
	/// モデル位置の設定
	/// </summary>
	void ModelTransfom(nlohmann::json& object, LevelData* levelData);
	/// <summary>
	/// 出現位置設定
	/// </summary>
	void SpawwnPoint(nlohmann::json& object, LevelData* levelData);
	/// <summary>
	/// カメラ位置
	/// </summary>
	void CameraTransform(nlohmann::json& object, LevelData* levelData);
	/// <summary>
	/// ライト位置
	/// </summary>
	void LightTransform(nlohmann::json& object, LevelData* levelData);
}

