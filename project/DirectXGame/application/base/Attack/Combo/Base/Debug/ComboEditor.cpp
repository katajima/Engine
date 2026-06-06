#include "ComboEditor.h"

#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"

#include <cctype>

namespace Combo {

#pragma region コンボエディター

	void Editor::Initialize(Engine::LineCommon* lineCommon,
		Combo::System* comboSystem, Engine::GlobalVariables* globalVariables,
		Character::BaseCharacter* owner, EffectSystem* effectSystem) {
		this->comboSystem = comboSystem;
		this->lineCommon = lineCommon;
		this->globalVariables = globalVariables;
		this->owner = owner;
		this->effectSystem = effectSystem;

		// コンボシステムからコンボエディターブロックを作成
		ApplyComboEditorToSystem();
	}

	void Editor::Update(float dt) {
#ifdef _DEBUG
		ImGui::Begin("Comdo");
		ImGui::Checkbox("isCreativeMode", &isComboEditorActive_);
		comboSystem->SertIsDebug(isComboEditorActive_);
		DrawNodeManagement();
		DrawStartComboSettings();
		// リロード
		if (ImGui::Button("Relord")) {
			Character::BasePlayer* player = dynamic_cast<Character::BasePlayer*>(owner);
			// グローバルデータ設定
			SetGlobalData();
			// リロード
			player->Reload();
			// コンボエディター
			ApplyComboEditorToSystem();
		}

		ImGui::Separator();
		if (ImGui::Button("SaveComboName")) {
			globalVariables->SaveFile(comboSystem->GetName());
		}
		ImGui::Separator();


		// 全てセーブ
		if (ImGui::Button("AllSave")) {
			SetGlobalData();
			globalVariables->SaveFile(comboSystem->GetName());
			for (auto& it : comboSystem->GetComboNodeStates()) {
				globalVariables->SaveFile(it.first);
			}
		}
		// セーブ
		if (ImGui::Button("Save")) {
			SetGlobalData();
			globalVariables->SaveFile(comboSystem->GetName());
			for (auto& it : comboSystem->GetComboNodeStates()) {
				if (it.first == selectedComboEditorBlockName_)
					globalVariables->SaveFile(it.first);
			}
		}


		if (isComboEditorActive_)
			UpdateImGui(dt);



		ImGui::End();
#endif // _DEBUG
	};

	void Editor::DrawNodeManagement() {
#ifdef _DEBUG
		ImGui::SeparatorText("Combo Nodes");
		ImGui::InputText("New Combo Name", newComboNameBuffer_.data(), newComboNameBuffer_.size());
		ImGui::SameLine();
		if (ImGui::Button("Add Combo")) {
			AddComboNode();
		}
		if (!comboEditorBlockNames_.empty()) {
			const char* preview = copySourceComboName_.empty() ? "コピー元を選択" : copySourceComboName_.c_str();
			if (ImGui::BeginCombo("Copy Source", preview)) {
				for (const std::string& comboName : comboEditorBlockNames_) {
					const bool selected = copySourceComboName_ == comboName;
					if (ImGui::Selectable(comboName.c_str(), selected)) {
						copySourceComboName_ = comboName;
					}
					if (selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			ImGui::Checkbox("接続もコピー", &copyConnections_);
			if (ImGui::Button("Add Copied Combo")) {
				AddCopiedComboNode();
			}
		}

		if (!selectedComboEditorBlockName_.empty()) {
			ImGui::Text("Selected: %s", selectedComboEditorBlockName_.c_str());
			if (ImGui::Button("Rename Selected Combo")) {
				pendingRenameComboName_ = selectedComboEditorBlockName_;
				strncpy_s(renameComboNameBuffer_.data(), renameComboNameBuffer_.size(),
					selectedComboEditorBlockName_.c_str(), _TRUNCATE);
				ImGui::OpenPopup("Confirm Combo Rename");
			}
			if (ImGui::Button("Delete Selected Combo")) {
				pendingDeleteComboName_ = selectedComboEditorBlockName_;
				ImGui::OpenPopup("Confirm Combo Delete");
			}
		}

		if (!nodeManagementMessage_.empty()) {
			ImGui::TextWrapped("%s", nodeManagementMessage_.c_str());
		}

		if (ImGui::BeginPopupModal("Confirm Combo Delete", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Delete combo '%s'?", pendingDeleteComboName_.c_str());
			ImGui::TextWrapped("This removes it from the combo list and deletes its saved JSON data.");
			ImGui::Separator();
			if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f))) {
				pendingDeleteComboName_.clear();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Delete Permanently", ImVec2(160.0f, 0.0f))) {
				DeleteComboNode(pendingDeleteComboName_);
				pendingDeleteComboName_.clear();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Confirm Combo Rename", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Rename combo '%s'", pendingRenameComboName_.c_str());
			ImGui::InputText("New Combo Name", renameComboNameBuffer_.data(), renameComboNameBuffer_.size());
			ImGui::TextWrapped("This updates saved data, start routes, and all connections to this combo.");
			ImGui::Separator();
			if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f))) {
				pendingRenameComboName_.clear();
				renameComboNameBuffer_.fill('\0');
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Apply Rename", ImVec2(160.0f, 0.0f))) {
				RenameComboNode(pendingRenameComboName_, renameComboNameBuffer_.data());
				if (nodeManagementMessage_.rfind("Renamed combo:", 0) == 0) {
					pendingRenameComboName_.clear();
					renameComboNameBuffer_.fill('\0');
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}
		ImGui::Separator();
#endif
	}

	void Editor::DrawStartComboSettings() {
#ifdef _DEBUG
		if (!ImGui::CollapsingHeader("Start Combo Settings")) {
			return;
		}

		StartComboRoutes routes = comboSystem->GetStartComboRoutes();
		bool changed = false;

		auto drawStartCombo = [&](const char* label, std::string& target) {
			const char* preview = target.empty() ? "未設定" : target.c_str();
			if (ImGui::BeginCombo(label, preview)) {
				for (const std::string& comboName : comboEditorBlockNames_) {
					const bool selected = target == comboName;
					if (ImGui::Selectable(comboName.c_str(), selected)) {
						target = comboName;
						changed = true;
					}
					if (selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		};

		ImGui::TextWrapped("攻撃を開始するときに使う最初のコンボを設定します。ここで変更した値は SaveComboName で ComboPlayer に保存されます。");
		drawStartCombo("地上 / 弱攻撃", routes.groundLight);
		drawStartCombo("空中 / 弱攻撃", routes.airLight);
		drawStartCombo("地上 / 強攻撃", routes.groundHeavy);
		drawStartCombo("空中 / 強攻撃", routes.airHeavy);
		drawStartCombo("地上 / スキル", routes.groundSkill);
		drawStartCombo("空中 / スキル", routes.airSkill);

		if (changed) {
			comboSystem->SetStartComboRoutes(routes);
		}
		ImGui::Separator();
#endif
	}

	void Editor::AddComboNode() {
		std::string comboName = newComboNameBuffer_.data();
		if (!ValidateNewComboName(comboName)) {
			return;
		}

		comboSystem->CreateCombo(comboName);
		globalVariables->SaveFile(comboName);
		globalVariables->SaveFile(comboSystem->GetName());
		ApplyComboEditorToSystem();
		selectedComboEditorBlockName_ = comboName;
		newComboNameBuffer_.fill('\0');
		nodeManagementMessage_ = "Added and saved combo: " + comboName;
	}

	void Editor::AddCopiedComboNode() {
		std::string comboName = newComboNameBuffer_.data();
		if (!ValidateNewComboName(comboName)) {
			return;
		}
		if (copySourceComboName_.empty()) {
			nodeManagementMessage_ = "Copy source combo is required.";
			return;
		}
		if (!globalVariables->HasKey(comboSystem->GetName(), copySourceComboName_) ||
			!comboSystem->GetComboNodeState(copySourceComboName_)) {
			nodeManagementMessage_ = "The copy source combo no longer exists.";
			return;
		}

		SetGlobalData();
		GlobalData copiedData = comboSystem->GetComboGlobalData(copySourceComboName_);
		if (!copyConnections_) {
			copiedData.connection = GlobalConnection{};
		}

		comboSystem->CreateCombo(comboName, copiedData);
		globalVariables->SaveFile(comboName);
		globalVariables->SaveFile(comboSystem->GetName());
		ApplyComboEditorToSystem();
		selectedComboEditorBlockName_ = comboName;
		newComboNameBuffer_.fill('\0');
		nodeManagementMessage_ = "Copied combo: " + copySourceComboName_ + " -> " + comboName;
	}

	bool Editor::ValidateNewComboName(const std::string& comboName) {
		if (comboName.empty()) {
			nodeManagementMessage_ = "Combo name is required.";
			return false;
		}
		for (const unsigned char c : comboName) {
			if (!std::isalnum(c) && c != '_') {
				nodeManagementMessage_ = "Use only letters, numbers, and underscores in combo names.";
				return false;
			}
		}
		if (globalVariables->HasKey(comboSystem->GetName(), comboName)) {
			nodeManagementMessage_ = "A combo with that name already exists.";
			return false;
		}
		if (globalVariables->HasGroup(comboName)) {
			nodeManagementMessage_ = "Saved data with that name already exists. Choose another name.";
			return false;
		}
		return true;
	}

	void Editor::RenameComboNode(const std::string& oldName, const std::string& newName) {
		if (oldName.empty() || !globalVariables->HasKey(comboSystem->GetName(), oldName)) {
			nodeManagementMessage_ = "The selected combo no longer exists.";
			return;
		}
		if (newName.empty()) {
			nodeManagementMessage_ = "Combo name is required.";
			return;
		}
		if (oldName == newName) {
			nodeManagementMessage_ = "Enter a different combo name.";
			return;
		}
		for (const unsigned char c : newName) {
			if (!std::isalnum(c) && c != '_') {
				nodeManagementMessage_ = "Use only letters, numbers, and underscores in combo names.";
				return;
			}
		}
		if (globalVariables->HasKey(comboSystem->GetName(), newName) ||
			globalVariables->HasGroup(newName)) {
			nodeManagementMessage_ = "A combo or saved data with that name already exists.";
			return;
		}

		SetGlobalData();
		if (!globalVariables->RenameGroup(oldName, newName)) {
			nodeManagementMessage_ = "Could not rename the saved combo data.";
			return;
		}

		globalVariables->RemoveItem(comboSystem->GetName(), oldName);
		globalVariables->SetValue(comboSystem->GetName(), newName, newName);
		comboSystem->RenameComboReferences(oldName, newName);

		for (const auto& node : comboSystem->GetComboNodeStates()) {
			if (node.first != oldName) {
				globalVariables->SaveFile(node.first);
			}
		}
		globalVariables->SaveFile(newName);
		globalVariables->SaveFile(comboSystem->GetName());
		globalVariables->RemoveSavedFile(oldName);

		Character::BasePlayer* player = dynamic_cast<Character::BasePlayer*>(owner);
		if (player) {
			player->Reload();
		}
		selectedComboEditorBlockName_ = newName;
		ApplyComboEditorToSystem();
		nodeManagementMessage_ = "Renamed combo: " + oldName + " -> " + newName;
	}

	void Editor::DeleteComboNode(const std::string& comboName) {
		if (comboName.empty() || !globalVariables->HasKey(comboSystem->GetName(), comboName)) {
			nodeManagementMessage_ = "The selected combo no longer exists.";
			return;
		}
		if (globalVariables->GetValue<std::string>(comboSystem->GetName(), kGroundLightStartKey) == comboName ||
			globalVariables->GetValue<std::string>(comboSystem->GetName(), kAirLightStartKey) == comboName ||
			globalVariables->GetValue<std::string>(comboSystem->GetName(), kGroundHeavyStartKey) == comboName ||
			globalVariables->GetValue<std::string>(comboSystem->GetName(), kAirHeavyStartKey) == comboName ||
			globalVariables->GetValue<std::string>(comboSystem->GetName(), kGroundSkillStartKey) == comboName ||
			globalVariables->GetValue<std::string>(comboSystem->GetName(), kAirSkillStartKey) == comboName) {
			nodeManagementMessage_ = "A configured start combo cannot be deleted because attacks must keep a valid entry point.";
			return;
		}

		for (auto& node : comboSystem->GetComboNodeStates()) {
			if (node.first == comboName) {
				continue;
			}
			GlobalData& data = comboSystem->GetComboGlobalData(node.first);
			bool changed = false;
			auto clearTarget = [&](std::string& target) {
				if (target == comboName) {
					target.clear();
					changed = true;
				}
			};
			auto clearConditional = [&](GlobalConditionalConnection& targets) {
				clearTarget(targets.groundMiss);
				clearTarget(targets.groundHit);
				clearTarget(targets.airMiss);
				clearTarget(targets.airHit);
			};
			clearTarget(data.connection.lightAttack);
			clearTarget(data.connection.heavyAttack);
			clearTarget(data.connection.skill);
			clearConditional(data.connection.lightCondition);
			clearConditional(data.connection.heavyCondition);
			clearConditional(data.connection.skillCondition);
			if (changed) {
				comboSystem->SetGlobalComboData(node.first, data);
				globalVariables->SaveFile(node.first);
			}
		}

		globalVariables->RemoveItem(comboSystem->GetName(), comboName);
		globalVariables->RemoveGroup(comboName);
		globalVariables->SaveFile(comboSystem->GetName());
		const bool removedFile = globalVariables->RemoveSavedFile(comboName);

		Character::BasePlayer* player = dynamic_cast<Character::BasePlayer*>(owner);
		if (player) {
			player->Reload();
		}
		ApplyComboEditorToSystem();
		nodeManagementMessage_ = removedFile
			? "Deleted combo: " + comboName
			: "Removed combo from the list, but its JSON file could not be deleted: " + comboName;
	}

	void Editor::UpdateImGui(float dt) {
#ifdef _DEBUG

		// 何もなければ何もしない
		if (comboEditorBlocks_.empty()) return;

		// 
		Engine::ImGuiManager::Select("Selected Combo", selectedComboEditorBlockName_, comboEditorBlocks_);

		ImGui::Separator();
		ImGui::Text("Editing: %s", selectedComboEditorBlockName_.c_str());


		// --- 選択されているブロックだけ表示 ---
		for (auto& combo : comboEditorBlocks_) {
			const std::string& name = combo.first;

			// ★ ここがポイント：選択中だけ true
			const bool nowChoice = (name == selectedComboEditorBlockName_);
			combo.second.SetNowChoice(nowChoice);

			// 選択中の1つだけ UpdateImGui 内部で描画される
			combo.second.UpdateImGui(dt, isComboEditorActive_);
		}
#endif // _DEBUG
	}

	void Editor::ApplyComboEditorToSystem() {
		// コンボエディターブロッククリア
		comboEditorBlocks_.clear();
		comboEditorBlockNames_.clear();

		// コンボシステムからコンボノードステートを取得してコンボエディターブロックを作成
		for (auto& comboState : comboSystem->GetComboNodeStates()) {
			CreateComboEditorBlock(comboState.second->GetName(), comboSystem, comboState.first, owner);
		}
		for (auto& combo : comboEditorBlocks_) {
			combo.second.SetConectComboNames(comboEditorBlockNames_);
		}

		// コンボ再構築後、選択が消えてたら補正
		if (!comboEditorBlocks_.empty()) {
			if (selectedComboEditorBlockName_.empty() ||
				comboEditorBlocks_.find(selectedComboEditorBlockName_) == comboEditorBlocks_.end()) {
				selectedComboEditorBlockName_ = comboEditorBlocks_.begin()->first;
			}
		}
		else {
			selectedComboEditorBlockName_.clear();
		}
	}

	void Editor::SetGlobalData() {

		for (auto& it : comboSystem->GetComboNodeStates()) {

			AttackSequence combo = comboEditorBlocks_[it.first].GetAttackSequence();
			GlobalData& data = comboSystem->GetComboGlobalData(it.first);


			// 条件
			{
				data.condition = comboEditorBlocks_[it.first].GetData().condition;
				data.action = comboEditorBlocks_[it.first].GetData().action;
				// 入力の時間
				data.condition.stateInput.startTime = ConvertUtility::FramesToSeconds(combo.GetEvent("入力の可能時間").startFrame);
				data.condition.stateInput.endTime = ConvertUtility::FramesToSeconds(combo.GetEvent("入力の可能時間").endFrame);
				// キャンセル可能時間
				data.condition.stateCancel.startTime = ConvertUtility::FramesToSeconds(combo.GetEvent("キャンセル可能時間").startFrame);
				data.condition.stateCancel.endTime = ConvertUtility::FramesToSeconds(combo.GetEvent("キャンセル可能時間").endFrame);
				// キャンセル可能時間(移動)
				data.condition.stateMoveCancel.startTime = ConvertUtility::FramesToSeconds(combo.GetEvent("移動キャンセル可能時間").startFrame);
				data.condition.stateMoveCancel.endTime = ConvertUtility::FramesToSeconds(combo.GetEvent("移動キャンセル可能時間").endFrame);
				// コンボ移行時間
				data.condition.stateNextTime = ConvertUtility::FramesToSeconds(combo.GetEvent("コンボ移行開始時間").startFrame);
				// ステート終了時間
				data.condition.stateEndTime = comboEditorBlocks_[it.first].GetMaxFrame();
			}

			// リアクション
			data.hitReaction = comboEditorBlocks_[it.first].GetData().hitReaction;
			// ヒットボックス生成時間
			float hitBoxStart = ConvertUtility::FramesToSeconds(combo.GetEvent("ヒットボックス生成時間").startFrame);
			data.hitBox = comboEditorBlocks_[it.first].GetData().hitBox;
			data.hitBox.windowStart = hitBoxStart;
			data.hitBox.lifeTime = ConvertUtility::FramesToSeconds(combo.GetEvent("ヒットボックス生成時間").endFrame) - hitBoxStart;

			// 移動時間
			data.move = comboEditorBlocks_[it.first].GetData().move;
			data.move.moveWindow.startTime = ConvertUtility::FramesToSeconds(combo.GetEvent("移動時間").startFrame);
			data.move.moveWindow.endTime = ConvertUtility::FramesToSeconds(combo.GetEvent("移動時間").endFrame);


			// アニメーションスピード
			data.animation = comboEditorBlocks_[it.first].GetData().animation;

			// カメラ
			data.camera = comboEditorBlocks_[it.first].GetData().camera;
			// エフェクト
			data.effect = comboEditorBlocks_[it.first].GetData().effect;
			data.effect.trailEffectStartTime = ConvertUtility::FramesToSeconds(combo.GetEvent("トレイルエフェクト時間").startFrame);
			data.effect.trailEffectLifeTime = ConvertUtility::FramesToSeconds(combo.GetEvent("トレイルエフェクト時間").endFrame) - data.effect.trailEffectStartTime;

			// 接続
			data.connection = comboEditorBlocks_[it.first].GetData().connection;
			// 攻撃タイプと遠距離攻撃
			data.type = comboEditorBlocks_[it.first].GetData().type;
			data.range = comboEditorBlocks_[it.first].GetData().range;

		}

		comboSystem->SetGlobalComboDatas();
	}

	void Editor::CreateComboEditorBlock(const std::string& comboName, Combo::System* comboSystem, const std::string& stateName, Character::BaseCharacter* owner) {

		// 既に存在する場合は追加しない
		if (comboEditorBlocks_.find(comboName) != comboEditorBlocks_.end()) {
			return;
		}

		// コンボエディターブロック作成
		EditorBlock block;
		block.Initialize(lineCommon, globalVariables, comboSystem, stateName, owner, effectSystem);

		// 名前リストに追加
		comboEditorBlockNames_.push_back(comboName);

		// マップに追加
		comboEditorBlocks_[comboName] = block;
	}

#pragma endregion // コンボエディター

}

