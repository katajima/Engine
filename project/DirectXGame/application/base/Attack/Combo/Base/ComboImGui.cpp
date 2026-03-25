#include "ComboImGui.h"

void Combo::ComboImGui::CurrentFrame(float dt, const AttackSequence& sequence, bool& isPlaying, bool& loopPlay,
	int& currentFrame, int& firstFrame, int& maxFrame) {
	// 現在のフレーム表示
	ImGui::Checkbox("再生するか", &isPlaying);
	ImGui::Checkbox("ループ再生", &loopPlay);
	ImGui::Separator();
	ImGui::Text("Current Frame: %d", currentFrame);
	ImGui::DragInt("最大フレーム", &maxFrame);
	ImGui::SliderInt("Current Frame", &currentFrame, 0, maxFrame);
	// 最初フレーム設定
	// 最初フレーム設定
	ImGui::DragInt("firstFrame ", &firstFrame, 1.0f, 0, maxFrame);
	// 最初のフレームが0未満にならないようにする
	if (0 > firstFrame) {
		firstFrame = 0;
	}
	ImGui::Separator();

	if (isPlaying) {
		currentFrame += static_cast<int>(dt * 60.0f); // 60FPS換算
	}

	// 最大値に行ったら戻す
	if (currentFrame >= sequence.GetFrameMax() && loopPlay) {
		currentFrame = 0;
	}
	else if (currentFrame >= sequence.GetFrameMax()) {
		currentFrame = sequence.GetFrameMax();
	}
}


void Combo::ComboImGui::ApplyAnimationToState(const std::string& imGuiName, std::string& animationName, int currentFrame,
	float& animationSpeed, float& animationBlendTime, const std::map<std::string, Engine::Animation>& animations,
	Engine::AnimationComponent* animation, const std::string& nowAnimationName) {

	if (ImGui::CollapsingHeader(imGuiName.c_str())) {
		ImGui::SliderFloat("アニメーションスピード", &animationSpeed, 0.1f, 10.0f, "%.2f");
		ImGui::SliderFloat("アニメーションブレンド時間", &animationBlendTime, 0.1f, 10.0f, "%.2f");

		// BeginComboでの選択
		ComboImGui::Select("Selected Combo",animationName, animations);
	}

	// ループ再生
	animation->SetIsLoop(false);
	// アニメーション設定
	animation->SetAnimation(nowAnimationName, 0);
	// ステートのアニメーション時間設定
	float animationTime = ConvertUtility::FramesToSeconds(currentFrame) * animationSpeed;
	animation->SetAnimationTime(animationTime);
}


#pragma region Sequence

void Combo::ComboImGui::SequenceSettings(AttackSequence& sequence, int& currentFrame, int& firstFrame, int& maxFrame, bool& expanded, int& selected) {
	// 最大フレーム設定
	sequence.SetFrameMax(maxFrame);

	// シーケンサー設定
	if (ImGui::CollapsingHeader("シーケンサー")) {
		ImGui::SliderFloat("Frame PixelWidthTarget",
			&ImSequencer::g_framePixelWidthTarget,
			1.0f, 40.0f, "%.1f px");
		ImGui::SliderFloat("Frame PixelWidth",
			&ImSequencer::g_framePixelWidth,
			1.0f, 40.0f, "%.1f px");
		ImGui::SliderInt("Frame LegendWidth",
			&ImSequencer::g_legendWidth,
			50, 500, "%d px");


		// シーケンサー表示
		ImGui::Separator();
		ImSequencer::Sequencer(
			&sequence,
			&currentFrame,
			&expanded,
			&selected,
			&firstFrame,
			ImSequencer::SEQUENCER_EDIT_ALL |
			ImSequencer::SEQUENCER_ADD |
			ImSequencer::SEQUENCER_DEL
		);
		ImGui::Separator();
	}
}

void Combo::ComboImGui::SequencerApplyToState(AttackSequence& sequence,ComboData& comboData, int& maxFrame) {

	// コンボ入力可能時間
	float inputStart = comboData.GetComboCondition().GetNextReceiver().GetInputStart();
	float inputEnd = comboData.GetComboCondition().GetNextReceiver().GetInputEnd();

	// コンボキャンセル時間
	float cancelStart = comboData.GetComboCondition().GetCancelReceiver().GetInputStart();
	float cancelEnd = comboData.GetComboCondition().GetCancelReceiver().GetInputEnd();
	// コンボキャンセル時間
	float cancelMoveStart = comboData.GetComboCondition().GetCancelReceiver().GetInputMoveStart();
	float cancelMoveEnd = comboData.GetComboCondition().GetCancelReceiver().GetInputMoveEnd();



	// コンボ移行時間
	float nextComboTime = comboData.GetComboCondition().GetNextCondition().GetData().stateTime;
	float endComboTime = comboData.GetComboCondition().GetEndCondition().GetData().stateTime;

	// ヒットボックス生成時間
	float hitBoxStart = comboData.GetComboHitBox().GetData().hitBpxWindowStart_;
	float hitBoxEnd = hitBoxStart + comboData.GetComboHitBox().GetData().lifeTime_;

	// コンボ終了時間
	maxFrame = ConvertUtility::SecondsToFrames(endComboTime, 60.0f);

	// 移動時間
	float moveStart = comboData.GetComboMotion().GetComboMove().GetData().moveWindowStart_;
	float moveEnd = comboData.GetComboMotion().GetComboMove().GetData().moveWindowEnd_;

	// トレイルエフェクト
	float trailStart = comboData.GetComboEffect().GetData().startTmer;
	float trailEnd = trailStart + comboData.GetComboEffect().GetData().lifeTime;

	AddSequencerEvent(sequence,inputStart, inputEnd, 0xFF00FF00, "入力の可能時間");
	AddSequencerEvent(sequence,cancelStart, cancelEnd, 0xFFFFFF00, "キャンセル可能時間");
	AddSequencerEvent(sequence,cancelMoveStart, cancelMoveEnd, 0xFFFFFF00, "移動キャンセル可能時間");
	AddSequencerEvent(sequence,nextComboTime, endComboTime, 0xFFFF0000, "コンボ移行開始時間");
	AddSequencerEvent(sequence,hitBoxStart, hitBoxEnd, 0x00FF0000, "ヒットボックス生成時間");
	AddSequencerEvent(sequence,moveStart, moveEnd, 0xFF000000, "移動時間");
	AddSequencerEvent(sequence,trailStart, trailEnd, 0x0000FF00, "トレイルエフェクト時間");
}

void Combo::ComboImGui::AddSequencerEvent(AttackSequence& sequence, float startFrame, float endFrame, 
	unsigned int color, const std::string& name) {
	sequence.AddEvent({ ConvertUtility::SecondsToFrames(startFrame,60.0f) , ConvertUtility::SecondsToFrames(endFrame,60.0f), color, name });
}

#pragma endregion // シーケンサー

