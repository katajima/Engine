#include "GlobalVariables.h"
#include "Windows.h"
#include "imgui.h"
#include <limits>



/// -----
/// 取得、生成、削除、検索
/// -----

/// <summary>
/// グループ生成
/// </summary>
void Engine::GlobalVariables::CreateGroup(const std::string& groupName) {
	// 指定名のオブジェクトがなければ追加する
	datas_[groupName];
}

/// <summary>
/// グループ検索
/// </summary>
bool Engine::GlobalVariables::HasGroup(const std::string& groupName) const
{
	return datas_.find(groupName) != datas_.end();
}

/// <summary>
/// キー検索
/// </summary>
bool Engine::GlobalVariables::HasKey(const std::string& groupName, const std::string& key) const
{
	auto groupIt = datas_.find(groupName);
	// 無いならfalse
	if (groupIt == datas_.end()) {
		return false;
	}
	const auto& group = groupIt->second;
	return group.find(key) != group.end();
}

/// <summary>
/// グループ名前取得
/// </summary>
std::vector<std::string> Engine::GlobalVariables::GetGroupNames() const
{
	std::vector<std::string> groupNames;
	groupNames.reserve(datas_.size());
	for (const auto& [groupName, _] : datas_) {
		groupNames.push_back(groupName);
	}
	return groupNames;
}

/// <summary>
/// キー名前取得
/// </summary>
std::vector<std::string> Engine::GlobalVariables::GetKeys(const std::string& groupName) const
{
	std::vector<std::string> keys;
	auto it = datas_.find(groupName);
	if (it == datas_.end()) return keys;

	const auto& group = it->second;
	keys.reserve(group.size());
	for (const auto& [key, _] : group) {
		keys.push_back(key);
	}
	return keys;
}

/// <summary>
/// 型タイプ取得
/// </summary>
std::string Engine::GlobalVariables::GetTypeName(const std::string& groupName, const std::string& key) const
{
	auto groupIt = datas_.find(groupName);
	if (groupIt == datas_.end()) {
		return "UnknownGroup";
	}

	const auto& group = groupIt->second;
	auto itemIt = group.find(key);
	if (itemIt == group.end()) {
		return "UnknownKey";
	}

	const auto& item = itemIt->second;

	// 各条件のどれかに当てはまっていたらタイプ設定
	if (std::holds_alternative<int32_t>(item)) return "int32_t";		// 
	if (std::holds_alternative<uint32_t>(item)) return "uint32_t";		// 
	if (std::holds_alternative<float>(item)) return "float";			// 
	if (std::holds_alternative<Vector2>(item)) return "Vector2";		// 
	if (std::holds_alternative<Vector3>(item)) return "Vector3";		// 
	if (std::holds_alternative<Vector4>(item)) return "Vector4";		// 
	if (std::holds_alternative<bool>(item)) return "bool";				// 
	if (std::holds_alternative<std::string>(item)) return "string";		// 
	if (std::holds_alternative<Transform>(item)) return "Transform";	// 
	if (std::holds_alternative<GvData::EnumItem>(item)) return "enum";

	return "UnknownType";
}

/// <summary>
/// アイテム削除
/// </summary>
void Engine::GlobalVariables::RemoveItem(const std::string& groupName, const std::string& key)
{
	auto groupIt = datas_.find(groupName);
	if (groupIt != datas_.end()) {
		groupIt->second.erase(key);

		// キーの順序リストからも削除
		auto& keys = groupKeys_[groupName];
		keys.erase(std::remove(keys.begin(), keys.end(), key), keys.end());
	}
}

/// <summary>
/// グループ削除
/// </summary>
void Engine::GlobalVariables::RemoveGroup(const std::string& groupName)
{
	datas_.erase(groupName);
	groupKeys_.erase(groupName);
}

/// <summary>
/// 複製
/// </summary>
bool Engine::GlobalVariables::DuplicateItem(const std::string& srcGroupName, const std::string& srcKey, const std::string& dstGroupName, const std::string& dstKey)
{
	// コピー元グループの存在チェック
	auto itSrcGroup = datas_.find(srcGroupName);
	if (itSrcGroup == datas_.end()) return false;

	auto& srcGroup = itSrcGroup->second;
	// コピー元キーの存在チェック
	auto itSrcItem = srcGroup.find(srcKey);
	if (itSrcItem == srcGroup.end()) return false;

	// コピー先グループを作成または取得
	auto itDstGroup = datas_.find(dstGroupName);
	if (itDstGroup == datas_.end()) {
		CreateGroup(dstGroupName);
		itDstGroup = datas_.find(dstGroupName);
		if (itDstGroup == datas_.end()) return false; // 失敗した場合
	}
	auto& dstGroup = itDstGroup->second;

	// コピー先キーが既にある場合は上書きしない
	if (dstGroup.find(dstKey) != dstGroup.end()) return false;

	// 複製
	dstGroup[dstKey] = itSrcItem->second;

	// groupKeys_の管理も忘れずに
	auto& dstKeys = groupKeys_[dstGroupName];
	dstKeys.push_back(dstKey);

	return true;
}

/// <summary>
/// 複製したときの重複対処
/// </summary>
std::string Engine::GlobalVariables::MakeUniqueKey(const std::string& baseKey, const GvData::Group& group)
{
	std::string newKey = baseKey;
	int count = 1;
	while (group.find(newKey) != group.end()) {
		newKey = baseKey + "(" + std::to_string(count++) + ")";
	}
	return newKey;
}




/// ------
/// セーブ、ロード、更新
/// ------

/// <summary>
/// セーブ
/// </summary>
void Engine::GlobalVariables::saveFile(const std::string& groupName) {

	// グループを検索
	std::map<std::string, GvData::Group>::iterator itGroup = datas_.find(groupName);
	// 未登録チェック
	assert(itGroup != datas_.end());

	GvData::json root;
	root = GvData::json::object();

	// jsonオブジェクト登録
	root[groupName] = GvData::json::object();

	// 各項目について
	for (std::map<std::string, GvData::Item>::iterator itItem = itGroup->second.begin(); itItem != itGroup->second.end(); ++itItem) {
		// 項目名を取得
		const std::string& itemName = itItem->first;
		// 項目の参照を取得
		GvData::Item& item = itItem->second;

		// 各型の値を保存
		GvFanction::Save(groupName, root, item, itemName);

	}
	// ディレクトリがなければ作成する
	// 書き込みはループの外で1回だけ行う
	std::filesystem::path dir(kDirectoryPath);
	if (!std::filesystem::exists(dir)) {
		std::filesystem::create_directory(dir);
	}

	// 書き込むJSONファイルのフルパスを合成する
	std::string filePath = kDirectoryPath + groupName + ".json";

	// 書き込み用ファイルストリーム
	std::ofstream ofs;
	// ファイルを書き込み用に開く
	ofs.open(filePath);
	// ファイルオープン失敗
	if (ofs.fail()) {
		std::string message = "Failed open data file for write.";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	// ファイルにjson文字列を書き込む(インデント幅4)
	ofs << std::setw(4) << root << std::endl;
	// ファイルを閉じる
	ofs.close();
}

/// <summary>
/// 全ロード
/// </summary>
void Engine::GlobalVariables::LoadFiles() {

	std::filesystem::path dir(kDirectoryPath);
	// ディレクトリがなければスキップする
	if (!std::filesystem::exists(kDirectoryPath)) {
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath);
	for (const std::filesystem::directory_entry& entry : dir_it) {
		// ファイルパスを取得
		const std::filesystem::path& filePath = entry.path();

		// ファイル拡張子を取得
		std::string extension = filePath.extension().string();
		// 　.jsonファイル以外はスキップ
		if (extension.compare(".json") != 0) {
			continue;
		}

		// ファイル読み込み
		LoadFile(filePath.stem().string());
	}
}

/// <summary>
/// グループロード
/// </summary>
void Engine::GlobalVariables::LoadFile(const std::string& groupName) {
	std::string filePath = kDirectoryPath + groupName + ".json";
	std::ifstream ifs(filePath);
	if (ifs.fail()) {
		MessageBoxA(nullptr, "Failed open data file for read.", "GlobalVariables", 0);
		assert(0);
		return;
	}

	GvData::json root;
	ifs >> root;
	ifs.close();

	auto itGroup = root.find(groupName);
	assert(itGroup != root.end());

	for (auto itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		const std::string& itemName = itItem.key();

		// 型情報があるかチェック
		if (itItem->is_object() && itItem->contains("type") && itItem->contains("value")) {
			std::string typeStr = itItem->at("type").get<std::string>();
			const auto& val = itItem->at("value");

			if (typeStr == "int32_t") {
				SetValue(groupName, itemName, val.get<int32_t>());
			}
			else if (typeStr == "uint32_t") {
				SetValue(groupName, itemName, val.get<uint32_t>());
			}
			else if (typeStr == "float") {
				SetValue(groupName, itemName, val.get<float>());
			}
			else if (typeStr == "Vector2") {
				Vector2 v = { val.at(0).get<float>(), val.at(1).get<float>() };
				SetValue(groupName, itemName, v);
			}
			else if (typeStr == "Vector3") {
				Vector3 v = { val.at(0).get<float>(), val.at(1).get<float>(), val.at(2).get<float>() };
				SetValue(groupName, itemName, v);
			}
			else if (typeStr == "Vector4") {
				Vector4 v = { val.at(0).get<float>(), val.at(1).get<float>(), val.at(2).get<float>(), val.at(3).get<float>() };
				SetValue(groupName, itemName, v);
			}
			else if (typeStr == "bool") {
				SetValue(groupName, itemName, val.get<bool>());
			}
			else if (typeStr == "string") {
				SetValue(groupName, itemName, val.get<std::string>());
			}
			else if (typeStr == "Transform") {
				const auto& val = (*itItem)["value"];
				Transform t;
				if (val.contains("scale") && val["scale"].is_array() && val["scale"].size() == 3) {
					t.scale = { val["scale"][0], val["scale"][1], val["scale"][2] };
				}
				if (val.contains("rotate") && val["rotate"].is_array() && val["rotate"].size() == 3) {
					t.rotate = { val["rotate"][0], val["rotate"][1], val["rotate"][2] };
				}
				if (val.contains("translate") && val["translate"].is_array() && val["translate"].size() == 3) {
					t.translate = { val["translate"][0], val["translate"][1], val["translate"][2] };
				}
				SetValue(groupName, itemName, t);
			}
			else if (typeStr == "enum") {
				GvData::EnumItem e;
				if (itItem->contains("enumType")) {
					e.enumType = (*itItem)["enumType"].get<std::string>();
				}
				e.value = val.get<int64_t>(); // "value" は int64 で読む
				// enum は内部表現として EnumItem を SetValue で入れる
				SetValue(groupName, itemName, e);
			}
			else {
				assert(false && "Unknown type found in JSON");
			}
		}
	}
}

/// <summary>
/// 更新
/// </summary>
void Engine::GlobalVariables::Update() {
#ifdef _DEBUG

	//ImGui::BeginMenuBar();

	static std::pair<std::string, std::string> pendingDeleteItem;
	static bool confirmDelete = false;

	std::vector<std::string> groupsToRemove;
	std::vector<std::pair<std::string, std::string>> itemsToRemove;

	ImGui::Begin("GlobalVariables");

	for (auto& [groupName, group] : datas_) {
	

		if (!ImGui::BeginMenu(groupName.c_str()))
			continue;

		for (auto& [itemName, item] : group) {
			ImGui::PushID(itemName.c_str());

			// 値表示・編集
			if (std::holds_alternative<int32_t>(item)) {
				ImGui::DragInt(itemName.c_str(), std::get_if<int32_t>(&item));
			}
			else if (std::holds_alternative<uint32_t>(item)) {
				uint32_t& value = std::get<uint32_t>(item);
				int32_t tmp = static_cast<int32_t>(value);
				if (ImGui::DragInt(itemName.c_str(), &tmp)) {
					if (tmp < 0) tmp = 0;
					value = static_cast<uint32_t>(tmp);
				}
			}
			else if (std::holds_alternative<float>(item)) {
				ImGui::DragFloat(itemName.c_str(), std::get_if<float>(&item), 0.01f);
			}
			else if (std::holds_alternative<Vector2>(item)) {
				ImGui::DragFloat2(itemName.c_str(), reinterpret_cast<float*>(std::get_if<Vector2>(&item)), 0.1f);
			}
			else if (std::holds_alternative<Vector3>(item)) {
				ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(std::get_if<Vector3>(&item)), 0.1f);
			}
			else if (std::holds_alternative<Vector4>(item)) {
				ImGui::DragFloat4(itemName.c_str(), reinterpret_cast<float*>(std::get_if<Vector4>(&item)), 0.1f);
			}
			else if (std::holds_alternative<bool>(item)) {
				ImGui::Checkbox(itemName.c_str(), std::get_if<bool>(&item));
			}
			else if (std::holds_alternative<std::string>(item)) {
				std::string* ptr = std::get_if<std::string>(&item);
				char buffer[256];
				strncpy_s(buffer, ptr->c_str(), sizeof(buffer));
				buffer[sizeof(buffer) - 1] = '\0';
				if (ImGui::InputText(itemName.c_str(), buffer, sizeof(buffer))) {
					*ptr = std::string(buffer);
				}
			}
			else if (std::holds_alternative<Transform>(item)) {
				Transform* ptr = std::get_if<Transform>(&item);
				ImGui::DragFloat3(("Scale##" + itemName).c_str(), reinterpret_cast<float*>(&ptr->scale), 0.1f);
				ImGui::DragFloat3(("Rotate##" + itemName).c_str(), reinterpret_cast<float*>(&ptr->rotate), 0.1f);
				ImGui::DragFloat3(("Translate##" + itemName).c_str(), reinterpret_cast<float*>(&ptr->translate), 0.1f);
			}
			else if (std::holds_alternative<GvData::EnumItem>(item)) {
				auto* e = std::get_if<GvData::EnumItem>(&item);

				// enumType 表示（必要なら編集も可）
				ImGui::Text("%s", itemName.c_str());
				ImGui::SameLine();
				ImGui::TextDisabled("(%s)", e->enumType.c_str());

				// 登録済み候補を探す
				const auto* entries = EnumRegistry::Instance().Find(e->enumType);

				if (entries && !entries->empty()) {
					// 現在の value に一致する index を探す
					int currentIndex = 0;
					for (int i = 0; i < (int)entries->size(); ++i) {
						if ((*entries)[i].value == e->value) {
							currentIndex = i;
							break;
						}
					}

					// Combo 用の表示名配列を作る（毎フレーム作るのが嫌ならキャッシュしてOK）
					std::vector<const char*> names;
					names.reserve(entries->size());
					for (auto& it : *entries) names.push_back(it.name);

					if (ImGui::Combo(itemName.c_str(), &currentIndex, names.data(), (int)names.size())) {
						e->value = (*entries)[currentIndex].value; // ★名前選択→値更新
					}
				}
				else {
					// 未登録なら仕方ないので数値編集（保険）
					int tmp = (int)e->value;
					if (ImGui::DragInt(itemName.c_str(), &tmp, 1.0f)) {
						e->value = (int64_t)tmp;
					}
					ImGui::SameLine();
					ImGui::TextDisabled("No enum entries registered.");
				}
			}


			ImGui::PopID();
		}

		ImGui::Text("\n");

		if (ImGui::Button("Save")) {
			saveFile(groupName);
			std::string message = std::format("{}.json saved.", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Group")) {
			groupsToRemove.push_back(groupName);
		}

		ImGui::EndMenu();

		
	}
	ImGui::End();

	//ImGui::EndMenuBar();




	// 削除予約された項目を後で実行（安全）
	for (const auto& [group, item] : itemsToRemove) {
		RemoveItem(group, item);
	}

	// 削除予約されたグループも後で実行
	for (const std::string& group : groupsToRemove) {
		RemoveGroup(group);
	}
#endif
}
