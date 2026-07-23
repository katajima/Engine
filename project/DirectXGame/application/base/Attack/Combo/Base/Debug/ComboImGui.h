#pragma once
#include "ComboSequencer.h"
#include "DirectXGame/application/base/Attack/Combo/Base/ComboSystem.h"
#include "DirectXGame/engine/Utility/ConvertUtility.h"
#include "DirectXGame/engine/Animation/Animation.h"

namespace Combo {
/// <summary>
/// ComboImGuiを管理・実装するクラス。
/// </summary>
	class ComboImGui {
	public:
		// 現在の時間
		static void CurrentFrame(float dt,bool isActive ,const AttackSequence& sequence, bool& isPlaying, bool& loopPlay,
			int& currentFrame, int& firstFrame, int& maxFrame);

		// アニメーション設定と表示
		static void ApplyAnimationToState(const std::string& name, bool isActive, int& currentFrame, int maxFrame, GlobalAnimation& animationData,const std::map<std::string, Engine::Animation>& animations,
			Engine::AnimationComponent* animation,const std::string& nowAnimationName);
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

}