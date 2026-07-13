#include "ComboEditor.h"
#include <DirectXGame/application/base/Character/Base/CharacterManager.h>
#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"
#include "DirectXGame/application/base/Attack/AttackController.h"
#include "imnodes.h"

#include <algorithm>
#include <cctype>

namespace Combo {

#pragma region コンボエディター

	void Editor::Initialize(Engine::LineCommon* lineCommon, Engine::GlobalVariables* globalVariables,
		Character::CharacterManager* characterManager, Character::BaseCharacter* owner, EffectSystem* effectSystem) {
		//	キャラクター管理
		this->characterManager = characterManager;
		// ライン描画共通
		this->lineCommon = lineCommon;
		// グローバル変数
		this->globalVariables = globalVariables;
		// エフェクトシステム
		this->effectSystem = effectSystem;
	}

	// 所有者設定
	void Editor::SetOwner(Character::BaseCharacter* owner) {
		// 所有者設定
		this->owner = owner;
		// 所有者の攻撃制御からコンボシステムを取得
		this->comboSystem = owner->GetAttackController()->GetComboSystem();
		// コンボシステムからコンボエディターブロックを作成
		ApplyComboEditorToSystem();
	}


	void Editor::Update(float dt) {
#ifdef _DEBUG
		ImGui::Begin("Comdo");
		ImGui::Checkbox("isCreativeMode", &isComboEditorActive_);
		comboSystem->SetIsDebug(isComboEditorActive_);

		// ノード管理の描画
		DrawNodeManagement();
		// 開始コンボ設定の描画
		DrawStartComboSettings();
		// ノードグラフ描画
		DrawComboNodeGraph();
		// セーブとリロードImGui描画
		DrawSaveComboNode();

		// 自動攻撃が停止中でも編集できるよう、詳細エディタのImGuiは常に更新する
		// isComboEditorActive_ はプレビュー再生と攻撃デバッグ処理の有効状態として渡す
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

	void Editor::DrawComboNodeGraph() {
#ifdef _DEBUG
		if (!ImGui::CollapsingHeader("Combo Node Graph", ImGuiTreeNodeFlags_DefaultOpen)) {
			return;
		}

		// 詳細エディタ側の未保存変更を保存データ側へ寄せて、グラフ表示の接続とSave時の接続を揃える。
		for (auto& [comboName, block] : comboEditorBlocks_) {
			if (comboSystem->GetComboNodeState(comboName)) {
				comboSystem->GetComboGlobalData(comboName).connection = block.GetMutableData().connection;
			}
		}

		struct LinkInfo {
			int id = 0;				// imnodesに渡すリンクID
			int startPinId = 0;		// 接続元ピンID
			int endPinId = 0;		// 接続先ピンID
		};

		auto getNodeId = [](int index) {
			// imnodes用に、ノードIDとピンIDの範囲を分けて衝突を避ける。
			return 1000 + index;
		};
		auto getInputPinId = [](int index) {
			// 各コンボノードへ入るための入力ピンID
			return 200000 + index;
		};
		auto getOutputPinId = [](int index, int slot) {
			// slot 0=弱攻撃, 1=強攻撃, 2=スキル
			return 300000 + index * 10 + slot;
		};
		auto getLinkId = [](int fromIndex, int slot) {
			// 標準接続は1ノードにつき3本までなので、接続元と入力種別で固定IDにする。
			return 400000 + fromIndex * 10 + slot;
		};
		auto getIndexFromInputPin = [](int pinId) {
			return pinId - 200000;
		};
		auto getIndexFromOutputPin = [](int pinId) {
			return (pinId - 300000) / 10;
		};
		auto getSlotFromOutputPin = [](int pinId) {
			return (pinId - 300000) % 10;
		};
		auto isInputPin = [](int pinId) {
			return pinId >= 200000 && pinId < 300000;
		};
		auto isOutputPin = [](int pinId) {
			return pinId >= 300000 && pinId < 400000;
		};

		std::map<std::string, int> comboNameToIndex;
		for (int i = 0; i < static_cast<int>(comboEditorBlockNames_.size()); ++i) {
			comboNameToIndex[comboEditorBlockNames_[i]] = i;
		}

		auto collectTarget = [&](std::vector<LinkInfo>& links, const std::string& from,
			const std::string& to, int slot) {
			const bool visibleLink = showAllNodeLinks_ || from == selectedComboEditorBlockName_ || to == selectedComboEditorBlockName_;
			if (!visibleLink || to.empty() ||
				comboNameToIndex.find(from) == comboNameToIndex.end() ||
				comboNameToIndex.find(to) == comboNameToIndex.end()) {
				return;
			}
			const int fromIndex = comboNameToIndex[from];
			const int toIndex = comboNameToIndex[to];
			links.push_back(LinkInfo{
				.id = getLinkId(fromIndex, slot),
				.startPinId = getOutputPinId(fromIndex, slot),
				.endPinId = getInputPinId(toIndex),
				});
		};
		auto drawConnectionCombo = [&](const char* label, std::string& target) {
			const char* preview = target.empty() ? "なし" : target.c_str();
			if (ImGui::BeginCombo(label, preview)) {
				const bool noSelection = target.empty();
				if (ImGui::Selectable("なし", noSelection)) {
					target.clear();
				}
				if (noSelection) {
					ImGui::SetItemDefaultFocus();
				}
				for (const std::string& comboName : comboEditorBlockNames_) {
					const bool selected = target == comboName;
					if (ImGui::Selectable(comboName.c_str(), selected)) {
						target = comboName;
					}
					if (selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		};

		std::vector<LinkInfo> links;
		ImGui::Checkbox("全ノードの線を表示", &showAllNodeLinks_);

		for (auto& [comboName, block] : comboEditorBlocks_) {
			GlobalConnection& connection = block.GetMutableData().connection;
			collectTarget(links, comboName, connection.lightAttack, 0);
			collectTarget(links, comboName, connection.heavyAttack, 1);
			collectTarget(links, comboName, connection.skill, 2);
		}

		ImGui::TextWrapped("ノードをクリックすると編集対象を切り替えます。ピン同士をドラッグすると標準接続を作成できます。");
		ImGui::Separator();

		const float nodeStartX = 40.0f;
		const float nodeStartY = 40.0f;
		const float nodeGapX = 300.0f;
		const float nodeGapY = 210.0f;
		const int columnCount = 3;
		ImNodes::BeginNodeEditor();
		for (const std::string& comboName : comboEditorBlockNames_) {
			const int nodeIndex = comboNameToIndex[comboName];
			const bool selected = comboName == selectedComboEditorBlockName_;

			const int nodeId = getNodeId(nodeIndex);
			// ノードの初期配置は最初の一度だけ行い、その後のドラッグ移動はimnodes側に保持させる。
			if (initializedNodeGraphNodeIds_.insert(nodeId).second) {
				ImNodes::SetNodeGridSpacePos(nodeId, ImVec2(
					nodeStartX + (nodeIndex % columnCount) * nodeGapX,
					nodeStartY + (nodeIndex / columnCount) * nodeGapY));
			}

			ImNodes::BeginNode(nodeId);
			ImNodes::BeginNodeTitleBar();
			ImGui::PushID(comboName.c_str());
			if (ImGui::Selectable(comboName.c_str(), selected, 0, ImVec2(180.0f, 22.0f))) {
				selectedComboEditorBlockName_ = comboName;
			}
			ImNodes::EndNodeTitleBar();

			ImNodes::BeginInputAttribute(getInputPinId(nodeIndex));
			ImGui::Text("In");
			ImNodes::EndInputAttribute();

			ImNodes::BeginOutputAttribute(getOutputPinId(nodeIndex, 0));
			ImGui::Indent(90.0f);
			ImGui::Text("弱攻撃");
			ImGui::Unindent(90.0f);
			ImNodes::EndOutputAttribute();

			ImNodes::BeginOutputAttribute(getOutputPinId(nodeIndex, 1));
			ImGui::Indent(90.0f);
			ImGui::Text("強攻撃");
			ImGui::Unindent(90.0f);
			ImNodes::EndOutputAttribute();

			ImNodes::BeginOutputAttribute(getOutputPinId(nodeIndex, 2));
			ImGui::Indent(90.0f);
			ImGui::Text("スキル");
			ImGui::Unindent(90.0f);
			ImNodes::EndOutputAttribute();

			ImGui::PopID();
			ImNodes::EndNode();
		}

		for (const LinkInfo& link : links) {
			ImNodes::Link(link.id, link.startPinId, link.endPinId);
		}
		ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight);
		ImNodes::EndNodeEditor();

		int startedPinId = 0;
		int endedPinId = 0;
		if (ImNodes::IsLinkCreated(&startedPinId, &endedPinId)) {
			// 逆向きにドラッグされた場合も、出力ピン -> 入力ピンとして解釈する。
			if (isInputPin(startedPinId) && isOutputPin(endedPinId)) {
				std::swap(startedPinId, endedPinId);
			}
			if (isOutputPin(startedPinId) && isInputPin(endedPinId)) {
				const int fromIndex = getIndexFromOutputPin(startedPinId);
				const int slot = getSlotFromOutputPin(startedPinId);
				const int toIndex = getIndexFromInputPin(endedPinId);
				if (fromIndex >= 0 && fromIndex < static_cast<int>(comboEditorBlockNames_.size()) &&
					toIndex >= 0 && toIndex < static_cast<int>(comboEditorBlockNames_.size()) &&
					slot >= 0 && slot < 3) {
					GlobalConnection& connection = comboEditorBlocks_[comboEditorBlockNames_[fromIndex]].GetMutableData().connection;
					std::string& target = slot == 0 ? connection.lightAttack :
						slot == 1 ? connection.heavyAttack : connection.skill;
					target = comboEditorBlockNames_[toIndex];
				}
			}
		}

		// グラフで選択中のノードについて、条件付き接続まで同じ場所で編集できるようにする。
		auto selectedIt = comboEditorBlocks_.find(selectedComboEditorBlockName_);
		if (selectedIt != comboEditorBlocks_.end()) {
			GlobalConnection& connection = selectedIt->second.GetMutableData().connection;
			auto drawConditionalConnection = [&](const char* inputLabel, GlobalConditionalConnection& targets) {
				ImGui::PushID(inputLabel);
				if (ImGui::TreeNode(inputLabel)) {
					ImGui::SetNextItemWidth(260.0f);
					drawConnectionCombo("地上 / 未ヒット", targets.groundMiss);
					ImGui::SetNextItemWidth(260.0f);
					drawConnectionCombo("地上 / ヒット", targets.groundHit);
					ImGui::SetNextItemWidth(260.0f);
					drawConnectionCombo("空中 / 未ヒット", targets.airMiss);
					ImGui::SetNextItemWidth(260.0f);
					drawConnectionCombo("空中 / ヒット", targets.airHit);
					ImGui::SetNextItemWidth(260.0f);
					drawConnectionCombo("ロックオン中", targets.lockOn);
					ImGui::SetNextItemWidth(260.0f);
					drawConnectionCombo("ロックオンなし", targets.noLockOn);
					ImGui::TreePop();
				}
				ImGui::PopID();
			};

			ImGui::SeparatorText("Selected Node Connections");
			ImGui::Text("Selected: %s", selectedComboEditorBlockName_.c_str());
			ImGui::SetNextItemWidth(260.0f);
			drawConnectionCombo("弱攻撃 / 標準", connection.lightAttack);
			ImGui::SetNextItemWidth(260.0f);
			drawConnectionCombo("強攻撃 / 標準", connection.heavyAttack);
			ImGui::SetNextItemWidth(260.0f);
			drawConnectionCombo("スキル / 標準", connection.skill);
			drawConditionalConnection("弱攻撃 / 条件付き", connection.lightCondition);
			drawConditionalConnection("強攻撃 / 条件付き", connection.heavyCondition);
			drawConditionalConnection("スキル / 条件付き", connection.skillCondition);
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
		drawStartCombo("回避中 / 弱攻撃", routes.dodgeLight);
		drawStartCombo("回避中 / 強攻撃", routes.dodgeHeavy);
		drawStartCombo("回避中 / スキル", routes.dodgeSkill);
		drawStartCombo("回避成功後 / 弱攻撃", routes.dodgeSuccessLight);
		drawStartCombo("回避成功後 / 強攻撃", routes.dodgeSuccessHeavy);
		drawStartCombo("回避成功後 / スキル", routes.dodgeSuccessSkill);

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
			globalVariables->GetValue<std::string>(comboSystem->GetName(), kAirSkillStartKey) == comboName ||
			globalVariables->GetValue<std::string>(comboSystem->GetName(), kDodgeLightStartKey) == comboName ||
			globalVariables->GetValue<std::string>(comboSystem->GetName(), kDodgeHeavyStartKey) == comboName ||
			globalVariables->GetValue<std::string>(comboSystem->GetName(), kDodgeSkillStartKey) == comboName ||
			globalVariables->GetValue<std::string>(comboSystem->GetName(), kDodgeSuccessLightStartKey) == comboName ||
			globalVariables->GetValue<std::string>(comboSystem->GetName(), kDodgeSuccessHeavyStartKey) == comboName ||
			globalVariables->GetValue<std::string>(comboSystem->GetName(), kDodgeSuccessSkillStartKey) == comboName) {
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
				clearTarget(targets.lockOn);
				clearTarget(targets.noLockOn);
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

	void Editor::DrawSaveComboNode() {
		// リロード
		if (ImGui::Button("Relord")) {
			// グローバルデータ設定
			SetGlobalData();
			// リロード
			owner->Reload();
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
		initializedNodeGraphNodeIds_.clear();

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
			// エディタで選択した音ID、再生時間、音量を保存データへ反映する。
			data.audio = comboEditorBlocks_[it.first].GetData().audio;
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
			for (int i = 0; i < static_cast<int>(data.effect.comboEffects.size()); ++i) {
				AttackEvent effectEvent{};
				if (combo.TryGetEvent(MakeComboEffectSequenceName(i), effectEvent)) {
					data.effect.comboEffects[i].startTime = ConvertUtility::FramesToSeconds(effectEvent.startFrame);
					data.effect.comboEffects[i].endTime = ConvertUtility::FramesToSeconds(effectEvent.endFrame);
					if (data.effect.comboEffects[i].endTime < data.effect.comboEffects[i].startTime) {
						data.effect.comboEffects[i].endTime = data.effect.comboEffects[i].startTime;
					}
				}
			}

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

