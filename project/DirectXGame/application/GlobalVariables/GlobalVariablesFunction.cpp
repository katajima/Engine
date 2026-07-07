#include "GlobalVariablesFunction.h"

void GvFunction::Save(const std::string& groupName, GvData::json& root, GvData::Item& item, const std::string& itemName)
{
	if (std::holds_alternative<int32_t>(item)) { // int32_t
		root[groupName][itemName] = {
		   {"type", "int32_t"},
		   {"value", std::get<int32_t>(item)}
		};
	}
	else if (std::holds_alternative<uint32_t>(item)) { // uint32_t
		root[groupName][itemName] = {
			{"type", "uint32_t"},
			{"value", std::get<uint32_t>(item)}
		};
	}
	else if (std::holds_alternative<float>(item)) { // float
		root[groupName][itemName] = {
		   {"type", "float"},
		   {"value", std::get<float>(item)}
		};
	}
	else if (std::holds_alternative<Vector2>(item)) { // Vector2
		Vector2 v = std::get<Vector2>(item);
		root[groupName][itemName] = {
			{"type", "Vector2"},
			{"value", { v.x, v.y }}
		};
	}
	else if (std::holds_alternative<Vector3>(item)) { // Vector3
		Vector3 v = std::get<Vector3>(item);
		root[groupName][itemName] = {
			{"type", "Vector3"},
			{"value", { v.x, v.y, v.z }}
		};
	}
	else if (std::holds_alternative<Vector4>(item)) { // Vector4
		Vector4 v = std::get<Vector4>(item);
		root[groupName][itemName] = {
			{"type", "Vector4"},
			{"value", { v.x, v.y, v.z, v.w }}
		};
	}
	else if (std::holds_alternative<bool>(item)) { // bool
		root[groupName][itemName] = {
		  {"type", "bool"},
		  {"value", std::get<bool>(item)}
		};
	}
	else if (std::holds_alternative<std::string>(item)) { // string
		root[groupName][itemName] = {
		  {"type", "string"},
		  {"value", std::get<std::string>(item)}
		};
	}
	else if (std::holds_alternative<Transform>(item)) { // Transform
		Transform v = std::get<Transform>(item);
		root[groupName][itemName] = {
			{"type", "Transform"},
			{"value", {
				{"scale", { v.scale.x, v.scale.y, v.scale.z }},
				{"rotate", { v.rotate.x, v.rotate.y, v.rotate.z }},
				{"translate", { v.translate.x, v.translate.y, v.translate.z }}
			}}
		};
	} 
	else if (std::holds_alternative<GvData::EnumItem>(item)) { // ★ EnumItem
		const auto& e = std::get<GvData::EnumItem>(item);
		root[groupName][itemName] = {
			{"type", "enum"},
			{"enumType", e.enumType},
			{"value", e.value}
		};
	}
}

void GvFunction::SetValue(const std::string& groupName, const std::string& itemName, const std::string& typeStr)
{

}
