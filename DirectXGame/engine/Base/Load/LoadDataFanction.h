#pragma once
#include "LevelData.h"
#include <json.hpp>
#include <vector>
#include <map>


namespace LoadDataFanc 
{

	nlohmann::json FileData(const std::string fullpath);

	void ModelTransfom(nlohmann::json& object, LevelData* levelData);

}

