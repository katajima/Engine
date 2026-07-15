#include "ComboImGui.h"
#include <DirectXGame/engine/Animation/AnimationComponent.h>
#include <algorithm>

void Combo::ComboImGui::CurrentFrame(float dt, bool isActive, const AttackSequence& sequence, bool& isPlaying, bool& loopPlay,
	int& currentFrame, int& firstFrame, int& maxFrame) {
	// 現在のフレーム表示
	if (isActive) {
		ImGui::Checkbox("再生するか", &isPlaying);
		ImGui::Checkbox("ループ再生", &loopPlay);
		ImGui::Separator();
		ImGui::Text("Current Frame: %d", currentFrame);
	}
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

	if (isPlaying && isActive) {
		currentFrame += static_cast<int>(dt * ConvertUtility::kDefaultFps); // 既定FPS換算
	}

	// 最大値に行ったら戻す
	if (currentFrame >= sequence.GetFrameMax() && loopPlay) {
		currentFrame = 0;
	}
	else if (currentFrame >= sequence.GetFrameMax()) {
		currentFrame = sequence.GetFrameMax();
	}
}


void Combo::ComboImGui::ApplyAnimationToState(const std::string& imGuiName,
	bool isActive, int& currentFrame, int maxFrame,GlobalAnimation& animationData, const std::map<std::string, Engine::Animation>& animations,
	Engine::AnimationComponent* animation, const std::string& nowAnimationName) {

	if (ImGui::CollapsingHeader(imGuiName.c_str())) {
		ImGui::SliderFloat("アニメーションスピード", &animationData.animationSpeed, 0.1f, 10.0f, "%.2f");
		ImGui::SliderFloat("アニメーションブレンド時間", &animationData.animationBlendTime, 0.0f, 10.0f, "%.2f");
		ImGui::Checkbox("アニメーションループ", &animationData.animationLoop);

		float endTime = static_cast<float>(maxFrame) / ConvertUtility::kDefaultFps;

		ImGui::SliderFloat("アニメーション停止タイミング", &animationData.animationStopTime, 0.0f, endTime, "%.2f");
		ImGui::Checkbox("アニメーションを一定時間で止めるか", &animationData.animationStop);

		// 攻撃前の接近や踏み込み中だけ別アニメーションを再生する設定
		ImGui::SeparatorText("攻撃前移動アニメーション");
		ImGui::Checkbox("攻撃前移動アニメーションを使用", &animationData.usePreMoveAnimation);
		if (animationData.usePreMoveAnimation) {
			Engine::ImGuiManager::Select("移動アニメーション", animationData.preMoveAnimationName, animations);
			ImGui::SliderFloat("移動アニメーション速度", &animationData.preMoveAnimationSpeed, 0.1f, 10.0f, "%.2f");
			ImGui::DragFloat("攻撃アニメーションへ切り替える時間", &animationData.preMoveAnimationEndTime, 0.01f, 0.0f, endTime, "%.2f");
			ImGui::SliderFloat("移動から攻撃へのブレンド時間", &animationData.preMoveAnimationBlendTime, 0.0f, 10.0f, "%.2f");
			ImGui::Checkbox("移動アニメーションループ", &animationData.preMoveAnimationLoop);
		}

		// キャラクター本体へ加算するTransformアニメーションを編集する
		ImGui::SeparatorText("トランスフォームアニメーション");
		ImGui::Checkbox("Transformアニメーションを使用", &animationData.isTransformAnimation);
		if (animationData.isTransformAnimation) {
			ImGui::DragFloat("Transform開始時間", &animationData.transformStartTime, 0.01f, 0.0f, endTime, "%.2f");
			ImGui::DragFloat("Transform終了時間", &animationData.transformEndTime, 0.01f, 0.0f, endTime, "%.2f");
			// 終了時間が開始時間より前にならないよう編集値を補正する
			animationData.transformEndTime = (std::max)(animationData.transformEndTime, animationData.transformStartTime);

			ImGui::DragFloat3("開始スケール加算値", &animationData.transformStart.scale.x, 0.01f);
			ImGui::DragFloat3("開始回転加算値(rad)", &animationData.transformStart.rotate.x, 0.01f);
			ImGui::DragFloat3("開始位置加算値", &animationData.transformStart.translate.x, 0.01f);
			ImGui::DragFloat3("終了スケール加算値", &animationData.transformEnd.scale.x, 0.01f);
			ImGui::DragFloat3("終了回転加算値(rad)", &animationData.transformEnd.rotate.x, 0.01f);
			ImGui::DragFloat3("終了位置加算値", &animationData.transformEnd.translate.x, 0.01f);
			ImGui::Checkbox("コンボ終了時にTransformを戻す", &animationData.restoreTransformOnExit);
		}

		// BeginComboでの選択
		Engine::ImGuiManager::Select("Selected Combo", animationData.animationName, animations);
	}

	if (isActive) {
		// 現在フレームを秒へ変換し、攻撃前移動アニメーションと攻撃アニメーションのプレビューを切り替える
		const float previewTime = ConvertUtility::FramesToSeconds(currentFrame);
		const bool previewPreMoveAnimation =
			animationData.usePreMoveAnimation &&
			!animationData.preMoveAnimationName.empty() &&
			previewTime < animationData.preMoveAnimationEndTime;

		if (previewPreMoveAnimation) {
			// 攻撃前移動時間中は、移動用アニメーションをその時間位置で表示する
			animation->SetIsLoop(animationData.preMoveAnimationLoop);
			animation->SetAnimation(animationData.preMoveAnimationName, 0);
			animation->SetAnimationTime(previewTime * animationData.preMoveAnimationSpeed);
		}
		else {
			// 攻撃アニメーションは移動アニメーション終了後を0秒として表示する
			const float attackPreviewTime = (std::max)(previewTime - animationData.preMoveAnimationEndTime, 0.0f);
			animation->SetIsLoop(false);
			animation->SetAnimation(nowAnimationName, 0);
			animation->SetAnimationTime(attackPreviewTime * animationData.animationSpeed);
		}
	}
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
	float inputStart = comboData.GetComboCondition().GetNextInputStart();
	float inputEnd = comboData.GetComboCondition().GetNextInputEnd();

	// コンボキャンセル時間
	float cancelStart = comboData.GetComboCondition().GetCancelInputStart();
	float cancelEnd = comboData.GetComboCondition().GetCancelInputEnd();
	// コンボキャンセル時間
	float cancelMoveStart = comboData.GetComboCondition().GetMoveCancelInputStart();
	float cancelMoveEnd = comboData.GetComboCondition().GetMoveCancelInputEnd();



	// コンボ移行時間
	float nextComboTime = comboData.GetComboCondition().GetData().stateNextTime;
	float endComboTime = comboData.GetComboCondition().GetData().stateEndTime;

	// ヒットボックス生成時間
	float hitBoxStart = comboData.GetComboHitBox().GetCollData().hitBoxData.windowStart;
	float hitBoxEnd = hitBoxStart + comboData.GetComboHitBox().GetCollData().hitBoxData.lifeTime;

	// コンボ終了時間
	maxFrame = ConvertUtility::SecondsToFrames(endComboTime, ConvertUtility::kDefaultFps);

	// 移動時間
	float moveStart = comboData.GetComboMotion().GetComboMove().GetData().moveWindow.startTime;
	float moveEnd = comboData.GetComboMotion().GetComboMove().GetData().moveWindow.endTime;

	// トレイルエフェクト
	float trailStart = comboData.GetComboEffect().GetData().trailEffectStartTime;
	float trailEnd = trailStart + comboData.GetComboEffect().GetData().trailEffectLifeTime;

	AddSequencerEvent(sequence,inputStart, inputEnd, 0xFF00FF00, "入力の可能時間");
	AddSequencerEvent(sequence,cancelStart, cancelEnd, 0xFFFFFF00, "キャンセル可能時間");
	AddSequencerEvent(sequence,cancelMoveStart, cancelMoveEnd, 0xFFFFFF00, "移動キャンセル可能時間");
	AddSequencerEvent(sequence,nextComboTime, endComboTime, 0xFFFF0000, "コンボ移行開始時間");
	AddSequencerEvent(sequence,hitBoxStart, hitBoxEnd, 0x00FF0000, "ヒットボックス生成時間");
	AddSequencerEvent(sequence,moveStart, moveEnd, 0xFF000000, "移動時間");
	AddSequencerEvent(sequence,trailStart, trailEnd, 0x0000FF00, "トレイルエフェクト時間");

	const auto& comboEffects = comboData.GetComboEffect().GetData().comboEffects;
	for (int i = 0; i < static_cast<int>(comboEffects.size()); ++i) {
		const ComboEffectEntry& entry = comboEffects[i];
		AddSequencerEvent(sequence, entry.startTime, entry.endTime, 0x00AAFFFF, MakeComboEffectSequenceName(i));
	}
}

void Combo::ComboImGui::AddSequencerEvent(AttackSequence& sequence, float startFrame, float endFrame,
	unsigned int color, const std::string& name) {
	sequence.AddEvent({ ConvertUtility::SecondsToFrames(startFrame, ConvertUtility::kDefaultFps) , ConvertUtility::SecondsToFrames(endFrame, ConvertUtility::kDefaultFps), color, name });
}

#pragma endregion // シーケンサー

