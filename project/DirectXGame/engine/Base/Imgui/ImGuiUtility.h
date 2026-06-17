#pragma once

#include "imgui.h"

#include <algorithm>
#include <array>
#include <string>
#include <vector>

namespace Engine::ImGuiUtility {

	// 固定長バッファを受け取り、std::arrayをそのままImGui::InputTextへ渡す。
	template<size_t Size>
	bool InputText(const char* label, std::array<char, Size>& buffer) {
		return ImGui::InputText(label, buffer.data(), buffer.size());
	}

	// 文字列を固定長バッファへコピーし、InputTextの初期値を安全に設定する。
	template<size_t Size>
	void SetInputTextBuffer(std::array<char, Size>& buffer, const std::string& text) {
		buffer.fill('\0');
		const size_t copySize = (std::min)(text.size(), buffer.size() - 1);
		std::copy_n(text.data(), copySize, buffer.data());
	}

	// 名前リストから現在選択中の文字列を切り替えるコンボボックスを描画する。
	bool SelectString(const char* label, std::string& selectedName,
		const std::vector<std::string>& names, const char* emptyPreview = "None");

	// enum値をラベル配列から選択するコンボボックスを描画する。
	template<typename Type, size_t N>
	bool SelectEnum(const char* label, const char* const (&labels)[N], Type& value) {
		int current = static_cast<int>(value);
		bool changed = false;

		// enum値がラベル範囲外の場合は先頭に戻し、表示クラッシュを避ける。
		if (current < 0 || current >= static_cast<int>(N)) {
			current = 0;
			value = static_cast<Type>(0);
			changed = true;
		}

		if (ImGui::BeginCombo(label, labels[current])) {
			for (int i = 0; i < static_cast<int>(N); ++i) {
				const bool selected = current == i;
				if (ImGui::Selectable(labels[i], selected)) {
					current = i;
					value = static_cast<Type>(i);
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

	// キー付きコンテナのキー一覧から現在選択中の文字列を切り替えるコンボボックスを描画する。
	template<typename Container>
	bool SelectMapKey(const char* label, std::string& selectedName,
		const Container& values, const char* emptyPreview = "None") {
		std::vector<std::string> names;

		// コンテナのキーだけを共通の文字列選択UIへ渡せる形に変換する。
		for (const auto& item : values) {
			names.push_back(item.first);
		}

		return SelectString(label, selectedName, names, emptyPreview);
	}

	// 直前の項目にヘルプツールチップを付ける。
	void HelpMarker(const char* text);

	// 削除やリネームの確認用モーダルを描画する。
	bool ConfirmModal(const char* popupName, const char* message,
		const char* confirmLabel = "OK", const char* cancelLabel = "Cancel");
}
