#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboSequencer.h"
#include "ComboSystem.h"
#include "DirectXGame/engine/Utility/ConvertUtility.h"
#include "DirectXGame/engine/Animation/Animation.h"

namespace Combo {
	class ComboImGui {
	public:
		// 現在の時間
		static void CurrentFrame(float dt, const AttackSequence& sequence, bool& isPlaying, bool& loopPlay,
			int& currentFrame, int& firstFrame, int& maxFrame);

		// アニメーション設定と表示
		static void ApplyAnimationToState(const std::string& name, std::string& animationName,int currentFrame,
			float& animationSpeed,float& animationBlendTime,const std::map<std::string, Engine::Animation>& animations,
			Engine::AnimationComponent* animation,const std::string& nowAnimationName);
	public: // BeginCombo
		// BeginComboを使用した選択ImGui
		template<typename Type, size_t N>
		static void Select(const std::string& name, const char* const (&label)[N], Type& type);

		// BeginComboでの選択(map<string,Type>)
		template<typename Type>
		static void Select(const std::string& name, std::string& animationName,const std::map<std::string, Type>& map);
	public: // シーケンサー
		// シーケンサー設定と表示
		static void SequenceSettings(AttackSequence& sequence, int& currentFrame, int& firstFrame, int& maxFrame,
			bool& expanded, int& selected);

		// シーケンサー適応
		static void SequencerApplyToState(AttackSequence& sequence, ComboData& comboData, int& maxFrame);
	private:
		// シーケンサー追加
		static void AddSequencerEvent(AttackSequence& sequence,float startFrame, float endFrame, 
			unsigned int color, const std::string& name);
	};

#pragma region MyRegion

	// BeginComboを使用した選択ImGui
	template<typename Type, size_t N>
	inline void ComboImGui::Select(const std::string& name, const char* const (&label)[N], Type& type) {
		int current = static_cast<int>(type);

		// 範囲外アクセス防止
		if (current < 0 || current >= static_cast<int>(N)) {
			current = 0;
			type = static_cast<Type>(0);
		}

		if (ImGui::BeginCombo(name.c_str(), label[current])) {
			for (int i = 0; i < static_cast<int>(N); ++i) {
				const bool isSelected = (current == i);

				if (ImGui::Selectable(label[i], isSelected)) {
					type = static_cast<Type>(i);
					current = i;
				}

				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

	// BeginComboでの選択(map<string,Type>)
	template<typename Type>
	inline void ComboImGui::Select(const std::string& beginComboName, std::string& animationName,const std::map<std::string, Type>& map) {
		
		// 初回：未選択なら先頭を選択
		if (animationName.empty()) {
			animationName = map.begin()->first;
		}
		// --- 選択UI（コンボボックス） ---
		const char* preview = animationName.c_str();
		if (ImGui::BeginCombo(beginComboName.c_str(), preview)) {

			for (auto& it : map) {
				const std::string& name = it.first;

				const bool isSelected = (name == animationName);
				if (ImGui::Selectable(name.c_str(), isSelected)) {
					animationName = name;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}
	}
#pragma endregion

}