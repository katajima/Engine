#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"
#include <iostream>
#include <fstream>
#include <variant>
#include <string>
#include <map>
#include<json.hpp>


namespace GvData {
	// 項目
	using Item = std::variant<int32_t, uint32_t, float, Vector2, Vector3, Vector4, bool, std::string, Transform>;

	// グループ
	using Group = std::map<std::string, Item>;

	// JSON
	using json = nlohmann::json;
}

namespace GvFanction {
	// 型を判別して値を保存
	void Save(const std::string& groupName, GvData::json& root,GvData::Item& item, const std::string& itemName);

	// 
	void SetValue(const std::string& groupName,const std::string& itemName,const std::string& typeStr);

}