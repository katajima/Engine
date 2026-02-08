#include "ComboEditor.h"

#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"

namespace Combo {

#pragma region ComboEditorBlock

	void EditorBlock::Initialize(Engine::GlobalVariables* globalVariables, Combo::System* comboSystem, std::shared_ptr<NodeState> state, BaseCharacter* owner) {
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

	void EditorBlock::UpdateImGui(float dt) {

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
		// シーケンサー設定
		ImGuiSequenceSettings();
		// シーケンサー表示
		SequencerProgress();
		// 移動関係設定
		ImGuiMove();
		// リアクション設定
		ImGuiReaction();
		// 終了条件
		ImGuiEndConditionType();
		// アニメーション
		ImGuiApplyAnimationToState();
		// ヒットボックス設定
		ImGuiApplyHitBox();


		// コンボデータを読み込んでステートに適応
		ApplyComboDataToState();
		// アニメーションの設定
		AnimationApplyToState();

		ImGui::End();

#endif // _DEBUG
	}

#pragma region ImGui

	void EditorBlock::ImGuiApplyAnimationToState() {
#ifdef _DEBUG
		if (ImGui::CollapsingHeader("アニメーション")) {
			ImGui::SliderFloat("アニメーションスピード", &data_.animationSpeed_, 0.1f, 10.0f, "%.2f");
			ImGui::SliderFloat("アニメーションブレンド時間", &data_.animationBlendTime_, 0.1f, 10.0f, "%.2f");



			// 初回：未選択なら先頭を選択
			if (data_.animationName.empty()) {
				data_.animationName = owner->GetObjectComponent()->GetObject3D()->GetModel()->modelData.animations.begin()->first;
			}

			// --- 選択UI（コンボボックス） ---
			{
				const char* preview = data_.animationName.c_str();
				if (ImGui::BeginCombo("Selected Combo", preview)) {

					for (auto& it : owner->GetObjectComponent()->GetObject3D()->GetModel()->modelData.animations) {
						const std::string& name = it.first;

						const bool isSelected = (name == data_.animationName);
						if (ImGui::Selectable(name.c_str(), isSelected)) {
							data_.animationName = name;
						}
						if (isSelected) {
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndCombo();
				}
			}
		}
#endif // _DEBUG
	}

	void EditorBlock::ImGuiApplyHitBox() {
		if (ImGui::CollapsingHeader("ヒットボックス")) {
			// ペアレント設定
			ImGuiApplyParentToState();
			// ヒットボックス出現条件
			ImGuiHitBoxSpawnType();
			// 依存先タイプ
			ImGuiHitBoxParentType();
			// オフセット
			ImGui::DragFloat3("オフセット", &data_.parentOffset_.x, 0.1f);
		}
	}

	void EditorBlock::ImGuiEndConditionType() {
		if (ImGui::CollapsingHeader("終了条件")) {
			static const char* EndConditionTypeLabels[] = {
			"着地したら",
			"離したら",
			"メータが空になったら",
			"時間経過",
			"当たったら",
			"特殊ケース",
			};


			int current = static_cast<int>(data_.endConditionType);

			if (ImGui::BeginCombo(
				"End Condition Type",
				EndConditionTypeLabels[current]
			)) {
				for (int i = 0; i < IM_ARRAYSIZE(EndConditionTypeLabels); ++i) {
					bool isSelected = (current == i);

					if (ImGui::Selectable(EndConditionTypeLabels[i], isSelected)) {
						data_.endConditionType = static_cast<Combo::EndConditionType>(i);
					}

					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}
	}

	void EditorBlock::ImGuiMove() {
		if (ImGui::CollapsingHeader("移動関係")) {
			static const char* MoveTypeLabels[] = {
			"無し",
			"ターゲットに向かって",
			"前方",
			};

			int current = static_cast<int>(data_.moveType);
			if (ImGui::BeginCombo(
				"移動タイプ",
				MoveTypeLabels[current]
			)) {
				for (int i = 0; i < IM_ARRAYSIZE(MoveTypeLabels); ++i) {
					bool isSelected = (current == i);

					if (ImGui::Selectable(MoveTypeLabels[i], isSelected)) {
						data_.moveType = static_cast<Combo::MoveType>(i);
					}

					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}


			ImGui::Checkbox("強制移動", &data_.isCompulsionMove_);
			ImGui::SliderFloat("移動速度", &data_.moveSpeed_, 0.0f, 100.0f, "%.2f");
			ImGui::Checkbox("重力", &data_.isGravity);
			ImGui::SliderFloat("重力倍率", &data_.gravityScale, 0.0f, 100.0f, "%.2f");
		}
	}

	void EditorBlock::ImGuiReaction() {
		if (ImGui::CollapsingHeader("リアクション")) {
			ImGui::SliderFloat("ノックバック持続時間", &data_.knockbackDuration_, 0.0f, 5.0f, "%.2f");
			ImGui::SliderFloat("ノックバックパワー", &data_.knockbackPower, 0.0f, 999.0f, "%.2f");
			ImGui::SliderFloat("ノックバックY方向パワー", &data_.knockbackPowerY, 0.0f, 999.0f, "%.2f");
			ImGui::Checkbox("Y方向にノックバック", &data_.isVerticalBoost_);
			ImGui::SliderFloat("ダメージ", &data_.damage, 0.0f, 1000.0f, "%.2f");
		}
	}

	void EditorBlock::ImGuiApplyParentToState() {


		// 初回：未選択なら先頭を選択
		if (data_.parentName_.empty()) {
			data_.parentName_ = owner->GetObjectComponent()->GetObject3D()->GetModel()->modelData.animations.begin()->first;
		}

		// --- 選択UI（コンボボックス） ---
		{
			const char* preview = data_.parentName_.c_str();
			if (ImGui::BeginCombo("依存先", preview)) {

				for (auto& it : comboSystem->GetParentTransforms()) {
					const std::string& name = it.first;

					const bool isSelected = (name == data_.parentName_);
					if (ImGui::Selectable(name.c_str(), isSelected)) {
						data_.parentName_ = name;
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}
		}
	}

	void EditorBlock::ImGuiFirstFrame() {

		// 最初フレーム設定
		ImGui::DragInt("firstFrame ", &firstFrame, 1.0f, 0, maxFrame);
		// 最初のフレームが0未満にならないようにする
		if (0 > firstFrame) {
			firstFrame = 0;
		}
	}

	void EditorBlock::ImGuiCurrentFrame(float dt) {
		// 現在のフレーム表示
		ImGui::Checkbox("再生するか", &isPlaying);
		ImGui::Checkbox("ループ再生", &loopPlay);
		ImGui::Separator();
		ImGui::Text("Current Frame: %d", currentFrame);
		ImGui::DragInt("最大フレーム", &maxFrame);
		ImGui::SliderInt("Current Frame", &currentFrame, 0, maxFrame);
		// 最初フレーム設定
		ImGuiFirstFrame();
		ImGui::Separator();

		if (isPlaying) {
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

	void EditorBlock::ImGuiHitBoxSpawnType() {
		static const char* HitBoxSpawnTypeLabels[] = {
		"時間経過",
		"着地",
		"空中",
		"ボタンを離したら",
		};
		int current = static_cast<int>(data_.spawnType_);

		if (ImGui::BeginCombo(
			"ヒットボックス出現条件",
			HitBoxSpawnTypeLabels[current]
		)) {
			for (int i = 0; i < IM_ARRAYSIZE(HitBoxSpawnTypeLabels); ++i) {
				bool isSelected = (current == i);

				if (ImGui::Selectable(HitBoxSpawnTypeLabels[i], isSelected)) {
					data_.spawnType_ = static_cast<HitBox::SpawnType>(i);
				}

				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

	}

	void EditorBlock::ImGuiHitBoxParentType() {
		static const char* HitBoxParentTypeLabels[] = {
		"親子付け",
		"孤立",
		"追従先からの孤立",
		"ターゲットの位置",
		};

		int current = static_cast<int>(data_.dependenceType_);

		if (ImGui::BeginCombo(
			"ヒットボックス依存先",
			HitBoxParentTypeLabels[current]
		)) {
			for (int i = 0; i < IM_ARRAYSIZE(HitBoxParentTypeLabels); ++i) {
				bool isSelected = (current == i);

				if (ImGui::Selectable(HitBoxParentTypeLabels[i], isSelected)) {
					data_.dependenceType_ = static_cast<HitBox::ParentType>(i);
				}

				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

	}

	void EditorBlock::ImGuiSequenceSettings() {
		if (ImGui::CollapsingHeader("シーケンサー設定")) {
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
	}

	void EditorBlock::AddSequencerEvent(float startFrame, float endFrame, unsigned int color, const std::string& name) {
		sequence_.AddEvent({ ConvertUtility::SecondsToFrames(startFrame,60.0f) , ConvertUtility::SecondsToFrames(endFrame,60.0f), color, name });
	}

	void EditorBlock::SequencerProgress() {
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

	void EditorBlock::AnimationApplyToState() {
		Engine::AnimationComponent* animation = owner->GetObjectComponent()->GetObject3D()->GetAnimationComponent();
		// ループ再生
		animation->SetIsLoop(false);
		// アニメーション設定
		animation->SetAnimation(state->GetAnimationName(), 0); // data_.animationName_;
		// ステートのアニメーション時間設定
		float animationTime = ConvertUtility::FramesToSeconds(currentFrame) * data_.animationSpeed_;
		animation->SetAnimationTime(animationTime);
	}

	void EditorBlock::ApplyComboDataToState() {
		// ステートのコンボデータ取得	
		ComboData& comboData = state->GetComboData();
		// ステートの時間設定
		comboData.SetTimer(ConvertUtility::FramesToSeconds(currentFrame, 60.0f));
	}

	void EditorBlock::SequencerApplyToState() {

		// ステートのコンボデータ取得	
		ComboData& comboData = state->GetComboData();

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
		data_.moveSpeed_ = comboData.GetComboMotion().GetComboMove().GetData().speed_;						// 移動速度
		data_.isCompulsionMove_ = comboData.GetComboMotion().GetComboMove().GetData().isCompulsionMove_;	// 強制移動
		data_.isGravity = comboData.GetComboMotion().GetComboMove().GetData().isGravity_;					// 重力
		data_.gravityScale = comboData.GetComboMotion().GetComboMove().GetData().gravityScale_;			// 重力スケール

		// リアクション
		data_.knockbackDuration_ = comboData.GetComboHitBox().GetCollData(0).reactionData.GetKnockbackData().GetData().duration_;	// ノックバック持続時間
		data_.knockbackPower = comboData.GetComboHitBox().GetCollData(0).reactionData.GetKnockbackData().GetData().power_;			// ノックバックパワー
		data_.knockbackPowerY = comboData.GetComboHitBox().GetCollData(0).reactionData.GetKnockbackData().GetData().verticalBoost_;		// ノックバックY方向パワー
		data_.isVerticalBoost_ = comboData.GetComboHitBox().GetCollData(0).reactionData.GetKnockbackData().GetData().isVerticalBoost_;	// Y方向にノックバック
		data_.damage = comboData.GetComboHitBox().GetCollData(0).reactionData.GetDamageData().GetOne().damage;						// ダメージ

		// アニメーションスピード
		data_.animationSpeed_ = comboData.GetComboMotion().GetComboAnimation().GetData().animationSpeed_;
		data_.animationBlendTime_ = comboData.GetComboMotion().GetComboAnimation().GetData().animationBlendTime_;
		// アニメーション名前
		data_.animationName = comboData.GetComboMotion().GetComboAnimation().GetData().animationName_;

		// トレイルエフェクト
		float trailStart = comboData.GetComboEffect().GetData().startTmer;
		float trailEnd = trailStart + comboData.GetComboEffect().GetData().lifeTime;

		// 親
		data_.parentName_ = comboData.GetComboHitBox().GetData().parentName_;
		data_.parentOffset_ = comboData.GetComboHitBox().GetData().offset_;
		data_.spawnType_ = comboData.GetComboHitBox().GetData().spawnType_;
		data_.dependenceType_ = comboData.GetComboHitBox().GetData().dependenceType_;

		// 終了条件
		data_.endConditionType = comboData.GetComboCondition().GetEndCondition().GetData().type;


		


		AddSequencerEvent(inputStart, inputEnd, 0xFF00FF00, "入力の可能時間");
		AddSequencerEvent(cancelStart, cancelEnd, 0xFFFFFF00, "キャンセル可能時間");
		AddSequencerEvent(cancelMoveStart, cancelMoveEnd, 0xFFFFFF00, "移動キャンセル可能時間");
		AddSequencerEvent(nextComboTime, endComboTime, 0xFFFF0000, "コンボ移行開始時間");
		AddSequencerEvent(hitBoxStart, hitBoxEnd, 0x00FF0000, "ヒットボックス生成時間");
		AddSequencerEvent(moveStart, moveEnd, 0xFF000000, "移動時間");
		AddSequencerEvent(trailStart, trailEnd, 0x0000FF00, "トレイルエフェクト時間");
	}

#pragma endregion // ImGui管理

#pragma endregion // コンボ単位管理


#pragma region コンボエディター

	void Editor::Initialize(Combo::System* comboSystem, Engine::GlobalVariables* globalVariables, BaseCharacter* owner) {
		this->comboSystem = comboSystem;
		this->globalVariables = globalVariables;
		this->owner = owner;


		// コンボシステムからコンボエディターブロックを作成
		ApplyComboEditorToSystem();
	}

	void Editor::Update(float dt) {
#ifdef _DEBUG
		ImGui::Begin("Comdo");
		ImGui::Checkbox("isCreativeMode", &isComboEditorActive_);

		// リロード
		if (ImGui::Button("Relord")) {
			BasePlayer* player = dynamic_cast<BasePlayer*>(owner);
			// グローバルデータ設定
			SetGlobalData();
			// リロード
			player->Reload();
			// コンボエディター
			ApplyComboEditorToSystem();
		}

		// セーブ
		if (ImGui::Button("Save")) {
			for (auto& it : comboSystem->GetComboNodeStates()) {
				globalVariables->SaveFile(it.first);
			}
		}
		ImGui::End();

		if (isComboEditorActive_)
			UpdateImGui(dt);
#endif // _DEBUG
	};

	void Editor::UpdateImGui(float dt) {
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

	void Editor::ApplyComboEditorToSystem() {
		// コンボエディターブロッククリア
		comboEditorBlocks_.clear();
		comboEditorBlockNames_.clear();

		// コンボシステムからコンボノードステートを取得してコンボエディターブロックを作成
		for (auto& comboState : comboSystem->GetComboNodeStates()) {
			CreateComboEditorBlock(comboState.second->GetName(), comboSystem, comboState.second, owner);
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
			GlovalData& data = comboSystem->GetComboGlobalData(it.first);


			// 入力の時間
			data.stateInputStartTime = ConvertUtility::FramesToSeconds(combo.GetEvent("入力の可能時間").startFrame);
			data.stateInputEndTime = ConvertUtility::FramesToSeconds(combo.GetEvent("入力の可能時間").endFrame);

			// キャンセル可能時間
			data.stateCancelStartTime = ConvertUtility::FramesToSeconds(combo.GetEvent("キャンセル可能時間").startFrame);
			data.stateCancelEndTime = ConvertUtility::FramesToSeconds(combo.GetEvent("キャンセル可能時間").endFrame);

			// キャンセル可能時間(移動)
			data.stateMoveCancelStartTime = ConvertUtility::FramesToSeconds(combo.GetEvent("移動キャンセル可能時間").startFrame);
			data.stateMoveCancelEndTime = ConvertUtility::FramesToSeconds(combo.GetEvent("移動キャンセル可能時間").endFrame);


			// コンボ移行時間
			data.stateNextTime = ConvertUtility::FramesToSeconds(combo.GetEvent("コンボ移行開始時間").startFrame);

			// ステート終了時間
			data.stateEndTime = comboEditorBlocks_[it.first].GetMaxFrame();

			// ヒットボックス生成時間
			float hitBoxStart = ConvertUtility::FramesToSeconds(combo.GetEvent("ヒットボックス生成時間").startFrame);
			data.hitBoxWindowStart_ = hitBoxStart;
			data.hitBoxLifeTime_ = ConvertUtility::FramesToSeconds(combo.GetEvent("ヒットボックス生成時間").endFrame) - hitBoxStart;

			// リアクション
			data.knockbackDuration_ = comboEditorBlocks_[it.first].GetData().knockbackDuration_;
			data.knockbackPower = comboEditorBlocks_[it.first].GetData().knockbackPower;
			data.knockbackPowerY = comboEditorBlocks_[it.first].GetData().knockbackPowerY;
			data.isVerticalBoost_ = comboEditorBlocks_[it.first].GetData().isVerticalBoost_;
			data.damage = comboEditorBlocks_[it.first].GetData().damage;

			// 移動時間
			data.moveWindowStart_ = ConvertUtility::FramesToSeconds(combo.GetEvent("移動時間").startFrame);
			data.moveWindowEnd_ = ConvertUtility::FramesToSeconds(combo.GetEvent("移動時間").endFrame);
			data.moveSpeed_ = comboEditorBlocks_[it.first].GetData().moveSpeed_;
			data.isCompulsionMove_ = comboEditorBlocks_[it.first].GetData().isCompulsionMove_;

			// 重力
			data.isGravity = comboEditorBlocks_[it.first].GetData().isGravity;
			data.gravityScale = comboEditorBlocks_[it.first].GetData().gravityScale;

			// アニメーションスピード
			data.animationSpeed_ = comboEditorBlocks_[it.first].GetData().animationSpeed_;
			data.animationBlendTime_ = comboEditorBlocks_[it.first].GetData().animationBlendTime_;
			data.animationName = comboEditorBlocks_[it.first].GetData().animationName;

			// エフェクト
			data.trailEffectStartTime = ConvertUtility::FramesToSeconds(combo.GetEvent("トレイルエフェクト時間").startFrame);
			data.trailEffectLifeTime = ConvertUtility::FramesToSeconds(combo.GetEvent("トレイルエフェクト時間").endFrame) - data.trailEffectStartTime;

			// ペアレント
			data.parentName_ = comboEditorBlocks_[it.first].GetData().parentName_;
			data.parentOffset_ = comboEditorBlocks_[it.first].GetData().parentOffset_;
			data.spawnType_ = comboEditorBlocks_[it.first].GetData().spawnType_;
			data.dependenceType_ = comboEditorBlocks_[it.first].GetData().dependenceType_;

			// 終了条件
			data.endConditionType = comboEditorBlocks_[it.first].GetData().endConditionType;

		}

		comboSystem->SetGlobalComboDatas();
	}

	void Editor::CreateComboEditorBlock(const std::string& comboName, Combo::System* comboSystem, std::shared_ptr<NodeState> state, BaseCharacter* owner) {

		// 既に存在する場合は追加しない
		if (comboEditorBlocks_.find(comboName) != comboEditorBlocks_.end()) {
			return;
		}

		// コンボエディターブロック作成
		EditorBlock block;
		block.Initialize(globalVariables, comboSystem, state, owner);

		// 名前リストに追加
		comboEditorBlockNames_.push_back(comboName);

		// マップに追加
		comboEditorBlocks_[comboName] = block;
	}

#pragma endregion // コンボエディター

}

