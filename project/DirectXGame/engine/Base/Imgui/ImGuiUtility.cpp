#include "ImGuiUtility.h"

namespace Engine::ImGuiUtility {

	bool SelectString(const char* label, std::string& selectedName,
		const std::vector<std::string>& names, const char* emptyPreview) {
		// 候補がない場合は未選択に戻し、空のコンボとして表示する。
		if (names.empty()) {
			selectedName.clear();
			ImGui::BeginDisabled();
			if (ImGui::BeginCombo(label, emptyPreview)) {
				ImGui::EndCombo();
			}
			ImGui::EndDisabled();
			return false;
		}

		bool changed = false;
		bool hasSelectedName = false;
		for (const std::string& name : names) {
			if (name == selectedName) {
				hasSelectedName = true;
				break;
			}
		}

		// 選択名が消えていた場合は先頭へ寄せ、以後の編集対象が空にならないようにする。
		if (!hasSelectedName) {
			selectedName = names.front();
			changed = true;
		}

		const char* preview = selectedName.empty() ? emptyPreview : selectedName.c_str();
		if (ImGui::BeginCombo(label, preview)) {
			for (const std::string& name : names) {
				const bool selected = selectedName == name;
				if (ImGui::Selectable(name.c_str(), selected)) {
					selectedName = name;
					changed = true;
				}
				if (selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		return changed;
	}

	void HelpMarker(const char* text) {
		// ラベル横へ控えめなヘルプを置き、ホバー時だけ詳細を表示する。
		ImGui::SameLine();
		ImGui::TextDisabled("(?)");
		if (ImGui::BeginItemTooltip()) {
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(text);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	bool ConfirmModal(const char* popupName, const char* message,
		const char* confirmLabel, const char* cancelLabel) {
		bool confirmed = false;
		if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::TextWrapped("%s", message);
			ImGui::Separator();
			if (ImGui::Button(cancelLabel, ImVec2(120.0f, 0.0f))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button(confirmLabel, ImVec2(160.0f, 0.0f))) {
				confirmed = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		return confirmed;
	}
}
