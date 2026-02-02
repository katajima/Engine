#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"
#include <iostream>
#include <fstream>
#include <variant>
#include <string>
#include <map>
#include<json.hpp>


struct EnumEntry {
	const char* name;   // 表示名
	int64_t value;      // EnumItem.value に入る値
};

class EnumRegistry {
public:
	static EnumRegistry& Instance() {
		static EnumRegistry inst;
		return inst;
	}

	void Register(const std::string& enumType, std::vector<EnumEntry> entries) {
		table_[enumType] = std::move(entries);
	}

	const std::vector<EnumEntry>* Find(const std::string& enumType) const {
		auto it = table_.find(enumType);
		if (it == table_.end()) return nullptr;
		return &it->second;
	}

private:
	std::unordered_map<std::string, std::vector<EnumEntry>> table_;
};

namespace GvData {

	// ★ enum保存用：型名 + 値（整数）
	struct EnumItem {
		std::string enumType; // 例: "EnemyType"
		int64_t value = 0;    // underlying をここに格納
	};

	// 項目
	using Item = std::variant<int32_t, uint32_t, float, Vector2, Vector3, Vector4, bool, std::string, Transform, EnumItem>;

	// グループ
	using Group = std::map<std::string, Item>;

	// JSON
	using json = nlohmann::json;
}

namespace GvFanction {
	// 型を判別して値を保存
	void Save(const std::string& groupName, GvData::json& root,GvData::Item& item, const std::string& itemName);

	// 値を設定
	void SetValue(const std::string& groupName,const std::string& itemName,const std::string& typeStr);

}