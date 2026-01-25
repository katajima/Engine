#include "ComboEditor.h"
#include "DirectXGame/engine/Utility/ConvertUtility.h"

#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"

#pragma region ComboEditorBlock

void ComboEditorBlock::Initialize(Engine::GlobalVariables* globalVariables, ComboSystem* comboSystem, std::shared_ptr<ComboNodeState> state, BaseCharacter* owner) {
	this->globalVariables = globalVariables;	// 保存項目
	this->state = state;						// ステート
	this->owner = owner;						// オーナー
	this->comboSystem = comboSystem;			// コンボシステム

	// シーケンサー初期化
	sequence_.ClearEvents();

	// 最大フレーム設定
	sequence_.SetFrameMax(maxFrame);

	// コンボ名設定
	comboName_ = "Combo_" + state->GetName();
	
	

	// シーケンサーにコンボデータを適用させる
	SequencerApplyToState();
	// コンボデータを読み込んでステートに適応
	ApplyComboDataToState();
}

void ComboEditorBlock::UpdateImGui(float dt) {

	// 選択中でなければ処理しない
	if (!nowChoice_) return;

	if (currentFrame == 0 && isPlaying) {
		comboSystem->GetComboStateMachine()->SetRoot(state);
	}
	state->SetTimeInState(ConvertUtility::FramesToSeconds(currentFrame, 60.0f));

#ifdef _DEBUG
	ImGui::Begin("Attack Editor");
	ImGui::Separator();
	ImGui::Text("Combo Name: %s", comboName_.c_str());
	ImGui::Separator();
	
	
	// 最大フレーム設定
	sequence_.SetFrameMax(maxFrame);

	// 現在のフレーム管理
	ImGuiCurrentFrame(dt);

	// 最初フレーム管理
	ImGuiFirstFrame();

	// シーケンサー設定
	ImGuiSequenceSettings();

	// シーケンサー表示
	SequencerProgress();
	
	// コンボデータを読み込んでステートに適応
	ApplyComboDataToState();

	// アニメーションの設定
	AnimationApplyToState();

	ImGui::End();

#endif // _DEBUG
}


// 開始
void ComboEditorBlock::Enter() {
	state->Enter(owner);
};
// 終了
void ComboEditorBlock::Exit() {
	state->End(owner);
};


#pragma region ImGui

void ComboEditorBlock::ImGuiFirstFrame() {

	// 最初フレーム設定
	ImGui::DragInt("firstFrame ", &firstFrame, 1.0f, 0, maxFrame);
	// 最初のフレームが0未満にならないようにする
	if (0 > firstFrame) {
		firstFrame = 0;
	}
}

void ComboEditorBlock::ImGuiCurrentFrame(float dt) {
	// 現在のフレーム表示
	ImGui::Checkbox("再生するか", &isPlaying);
	ImGui::Checkbox("ループ再生", &loopPlay);
	ImGui::Text("Current Frame: %d", currentFrame);
	ImGui::DragInt("最大フレーム", &maxFrame);
	ImGui::SliderInt("Current Frame", &currentFrame,0,maxFrame);


	if(isPlaying){
		currentFrame += static_cast<int>(dt * 60.0f); // 60FPS換算
	}


	


	// 最大値に行ったら戻す
	if (currentFrame >= sequence_.GetFrameMax() && loopPlay) {
		currentFrame = 0;
	}
	else if (currentFrame >= sequence_.GetFrameMax()) {
		currentFrame = sequence_.GetFrameMax();
	}
}

void ComboEditorBlock::ImGuiSequenceSettings() {
	ImGui::SliderFloat("Frame PixelWidthTarget",
		&ImSequencer::g_framePixelWidthTarget,
		1.0f, 40.0f, "%.1f px");
	ImGui::SliderFloat("Frame PixelWidth",
		&ImSequencer::g_framePixelWidth,
		1.0f, 40.0f, "%.1f px");
	ImGui::SliderInt("Frame LegendWidth",
		&ImSequencer::g_legendWidth,
		50, 500, "%d px");
}

void ComboEditorBlock::AddSequencerEvent(float startFrame, float endFrame, unsigned int color, const std::string& name){
	sequence_.AddEvent({ ConvertUtility::SecondsToFrames(startFrame,60.0f) , ConvertUtility::SecondsToFrames(endFrame,60.0f), color, name });
}

void ComboEditorBlock::SequencerProgress(){
	ImGui::Separator();
	ImSequencer::Sequencer(
		&sequence_,
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

void ComboEditorBlock::AnimationApplyToState(){
	Engine::AnimationComponent* animation = owner->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
	animation->SetIsLoop(false);
	// アニメーション設定
	animation->SetAnimation(state->GetAnimationName(), 0);
	// ステートのアニメーション時間設定
	animation->SetAnimationTime(ConvertUtility::FramesToSeconds(currentFrame));
}

void ComboEditorBlock::ApplyComboDataToState(){
	// ステートのコンボデータ取得	
	ComboData& comboData = state->GetComboData();
	// ステートの時間設定
	comboData.SetTimer(ConvertUtility::FramesToSeconds(currentFrame, 60.0f));
}

void ComboEditorBlock::SequencerApplyToState(){

	// ステートのコンボデータ取得	
	ComboData& comboData = state->GetComboData();
	
	// コンボ入力可能時間
	float inputStart = comboData.GetComboCondition().GetComboInputStart();
	float inputEnd = comboData.GetComboCondition().GetComboInputEnd();

	// コンボキャンセル時間
	float cancelStart = comboData.GetComboCondition().GetInput().cancelStart_;
	float cancelEnd = comboData.GetComboCondition().GetInput().cancelEnd_;

	// コンボ移行時間
	float nextComboTime = comboData.GetComboCondition().GetComboNextTime();
	float endComboTime = comboData.GetComboCondition().GetData().stateEndTime;

	// ヒットボックス生成時間
	float hitBoxStart = comboData.GetComboHitBox().GetData().hitBpxWindowStart_;
	float hitBoxEnd = hitBoxStart + comboData.GetComboHitBox().GetData().lifeTime_ + comboData.GetComboHitBox().GetData().hitBpxWindowStart_;

	// コンボ終了時間
	maxFrame = ConvertUtility::SecondsToFrames(endComboTime, 60.0f);


	// 移動時間
	float moveStart = comboData.GetComboMotion().GetData().moveWindowStart_;
	float moveEnd = comboData.GetComboMotion().GetData().moveWindowEnd_;


	AddSequencerEvent(inputStart, inputEnd, 0xFF00FF00, "入力の可能時間");
	AddSequencerEvent(cancelStart, cancelEnd, 0xFFFFFF00, "キャンセル可能時間");
	AddSequencerEvent(nextComboTime, endComboTime, 0xFFFF0000, "コンボ移行開始時間");
	AddSequencerEvent(hitBoxStart, hitBoxEnd, 0xFFFF0000, "ヒットボックス生成時間");
	AddSequencerEvent(moveStart, moveEnd, 0xFFFF0000, "移動時間");

}

#pragma endregion // ImGui管理

#pragma endregion // コンボ単位管理


#pragma region コンボエディター

void ComboEditor::Initialize(ComboSystem* comboSystem, Engine::GlobalVariables* globalVariables, BaseCharacter* owner){
	this->comboSystem = comboSystem;
	this->globalVariables = globalVariables;
	this->owner = owner;


	// コンボシステムからコンボエディターブロックを作成
	ApplyComboEditorToSystem();
}

void ComboEditor::UpdateImGui(float dt) {
#ifdef _DEBUG

	// 何もなければ何もしない
	if (comboEditorBlocks_.empty()) return;

	ImGui::Begin("Combo Editor");

	// 初回：未選択なら先頭を選択
	if (selectedComboEditorBlockName_.empty()) {
		selectedComboEditorBlockName_ = comboEditorBlocks_.begin()->first;
	}

	// --- 選択UI（コンボボックス） ---
	{
		const char* preview = selectedComboEditorBlockName_.c_str();
		if (ImGui::BeginCombo("Selected Combo", preview)) {

			for (auto& it : comboEditorBlocks_) {
				const std::string& name = it.first;

				const bool isSelected = (name == selectedComboEditorBlockName_);
				if (ImGui::Selectable(name.c_str(), isSelected)) {
					selectedComboEditorBlockName_ = name;
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}
	}

	ImGui::Separator();
	ImGui::Text("Editing: %s", selectedComboEditorBlockName_.c_str());

	ImGui::End();

	// --- 選択されているブロックだけ表示 ---
	for (auto& combo : comboEditorBlocks_) {
		const std::string& name = combo.first;

		// ★ ここがポイント：選択中だけ true
		const bool nowChoice = (name == selectedComboEditorBlockName_);
		combo.second.SetNowChoice(nowChoice);

		// 選択中の1つだけ UpdateImGui 内部で描画される
		combo.second.UpdateImGui(dt);
	}
#endif // _DEBUG
}

void ComboEditor::ApplyComboEditorToSystem(){
	// コンボエディターブロッククリア
	comboEditorBlocks_.clear();

	// コンボシステムからコンボノードステートを取得してコンボエディターブロックを作成
	for (auto& comboState : comboSystem->GetComboNodeStates()) {
		CreateComboEditorBlock(comboState.second->GetName(), comboSystem, comboState.second,owner);
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

void ComboEditor::CreateComboEditorBlock(const std::string& comboName, ComboSystem* comboSystem, std::shared_ptr<ComboNodeState> state, BaseCharacter* owner){

	// 既に存在する場合は追加しない
	if (comboEditorBlocks_.find(comboName) != comboEditorBlocks_.end()) {
		return;
	}

	// コンボエディターブロック作成
	ComboEditorBlock block;
	block.Initialize(globalVariables, comboSystem, state, owner);

	// マップに追加
	comboEditorBlocks_[comboName] = block;
}

#pragma endregion // コンボエディター



