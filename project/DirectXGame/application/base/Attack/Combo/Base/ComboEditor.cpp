#include "ComboEditor.h"

#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"

namespace Combo {

#pragma region ComboEditorBlock

	void EditorBlock::Initialize(Engine::LineCommon* lineCommon, Engine::GlobalVariables* globalVariables, Combo::System* comboSystem, std::shared_ptr<NodeState> state, Character::BaseCharacter* owner) {
		this->globalVariables = globalVariables;	// 保存項目
		this->lineCommon = lineCommon;
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
		// ステートのコンボデータ取得	
		ComboData& comboData = state->Data();
		// ステートの時間設定
		comboData.SetTimer(ConvertUtility::FramesToSeconds(currentFrame, 60.0f));
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

		// 現在の時間
		ComboImGui::CurrentFrame(dt, sequence_, isPlaying, loopPlay, currentFrame, firstFrame, maxFrame);

		// シーケンサーの設定と表示
		ComboImGui::SequenceSettings(sequence_, currentFrame, firstFrame, maxFrame, expanded, selected);

		// アニメーション
		ComboImGui::ApplyAnimationToState("アニメーション", data_.animation.animationName, currentFrame, data_.animation.animationSpeed,
			data_.animation.animationBlendTime, owner->GetObjectComponent()->GetObject3D()->GetModel()->GetModelData().animations,
			owner->GetObjectComponent()->GetObject3D()->GetAnimationComponent(), state->GetAnimationName());


		// 移動関係設定
		ImGuiMove();
		// ロックオン関係設定
		ImGuiLockOn();
		// リアクション設定
		ImGuiReaction();
		// 終了条件
		ImGuiEndConditionType();
		// ヒットボックス設定
		ImGuiApplyHitBox();


		// ステートのコンボデータ取得	
		ComboData& comboData = state->Data();
		// ステートの時間設定
		comboData.SetTimer(ConvertUtility::FramesToSeconds(currentFrame, 60.0f));
		ImGui::End();

#endif // _DEBUG
	}

#pragma region ImGui

	void EditorBlock::ImGuiApplyHitBox() {
		if (ImGui::CollapsingHeader("ヒットボックス")) {
			// ペアレント設定
			ComboImGui::Select("依存先", data_.hitBox.parentName, comboSystem->GetParentTransforms());

			// ヒットボックス出現条件
			static const char* HitBoxSpawnTypeLabels[] = {
				"時間経過",
				"着地",
				"空中",
				"ボタンを離したら",
			};
			ComboImGui::Select("ヒットボックス出現条件", HitBoxSpawnTypeLabels, data_.hitBox.spawnType);

			// 依存先タイプ
			static const char* HitBoxParentTypeLabels[] = {
				"親子付け",
				"孤立",
				"追従先からの孤立",
				"ターゲットの位置",
			};
			ComboImGui::Select("ヒットボックス依存先", HitBoxParentTypeLabels, data_.hitBox.dependenceType);
			
			// 影響タイプ
			static const char* HitBoxHitEffectTypeLabels[] = {
				"ダメージのみ",
				"力の影響のみ",
				"ダメージと力の影響",
			};
			ComboImGui::Select("ヒットボックス影響", HitBoxHitEffectTypeLabels, data_.hitBox.hitEffectType);

			// 生存タイプ
			static const char* HitBoxLifetimeTypeLabels[] = {
				"無期限",
				"期限付き",
			};
			ComboImGui::Select("ヒットボックス生存", HitBoxLifetimeTypeLabels, data_.hitBox.lifetimeType);

			// オフセット
			ImGui::DragFloat3("オフセット", &data_.hitBox.parentOffset.x, 0.1f);
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
			ComboImGui::Select("終了条件タイプ", EndConditionTypeLabels, data_.endConditionType);
		}
	}

	void EditorBlock::ImGuiMove() {
		if (ImGui::CollapsingHeader("移動関係")) {
			static const char* MoveTypeLabels[] = {
			"無し",
			"ターゲットに向かって",
			"前方",
			"カメラ方向",
			};
			ComboImGui::Select("移動タイプ", MoveTypeLabels, data_.move.moveType);

			ImGui::Checkbox("強制移動", &data_.move.isCompulsionMove);
			ImGui::SliderFloat("移動速度", &data_.move.moveSpeed, 0.0f, 100.0f, "%.2f");
			ImGui::Checkbox("重力", &data_.move.isGravity);
			ImGui::SliderFloat("重力倍率", &data_.move.gravityScale, 0.0f, 100.0f, "%.2f");
		}
	}

	void EditorBlock::ImGuiLockOn() {
		if (ImGui::CollapsingHeader("ロックオン関係")) {
			static const char* LockOnTypeLabels[] = {
			"当てた相手",
			"近い相手",
			};
			ComboImGui::Select("ロックオンタイプ", LockOnTypeLabels, data_.lockOn.lockOnType);
			ImGui::SliderFloat("ソフトロックオン範囲", &data_.lockOn.lockOnRadius, 0.0f, 100.0f);
		}
	}

	void EditorBlock::ImGuiReaction() {
		if (ImGui::CollapsingHeader("リアクション")) {
			ImGui::SliderFloat("ノックバック持続時間", &data_.reaction.knockbackDuration, 0.0f, 5.0f, "%.2f");
			ImGui::SliderFloat("ノックバックパワー", &data_.reaction.knockbackPower, 0.0f, 999.0f, "%.2f");
			ImGui::SliderFloat("ノックバックY方向パワー", &data_.reaction.knockbackPowerY, 0.0f, 999.0f, "%.2f");
			ImGui::Checkbox("Y方向にノックバック", &data_.reaction.isVerticalBoost);
			ImGui::SliderFloat("ダメージ", &data_.reaction.damage, 0.0f, 1000.0f, "%.2f");
		}
	}

	
	void EditorBlock::SequencerApplyToState() {

		// ステートのコンボデータ取得	
		ComboData& comboData = state->Data();


		data_.move.moveSpeed = comboData.GetComboMotion().GetComboMove().GetData().speed_;						// 移動速度
		data_.move.isCompulsionMove = comboData.GetComboMotion().GetComboMove().GetData().isCompulsionMove_;	// 強制移動
		data_.move.isGravity = comboData.GetComboMotion().GetComboMove().GetData().isGravity_;					// 重力
		data_.move.gravityScale = comboData.GetComboMotion().GetComboMove().GetData().gravityScale_;				// 重力スケール
		data_.move.moveType = comboData.GetComboMotion().GetComboMove().GetData().moveType;						// 移動タイプ


		// リアクション
		data_.reaction.knockbackDuration = comboData.GetComboHitBox().GetCollData(0).reactionData.GetKnockbackData().GetData().duration_;	// ノックバック持続時間
		data_.reaction.knockbackPower = comboData.GetComboHitBox().GetCollData(0).reactionData.GetKnockbackData().GetData().power_;			// ノックバックパワー
		data_.reaction.knockbackPowerY = comboData.GetComboHitBox().GetCollData(0).reactionData.GetKnockbackData().GetData().verticalBoost_;		// ノックバックY方向パワー
		data_.reaction.isVerticalBoost = comboData.GetComboHitBox().GetCollData(0).reactionData.GetKnockbackData().GetData().isVerticalBoost_;	// Y方向にノックバック
		data_.reaction.damage = comboData.GetComboHitBox().GetCollData(0).reactionData.GetDamageData().GetOne().damage;						// ダメージ

		// アニメーションスピード
		data_.animation.animationSpeed = comboData.GetComboMotion().GetComboAnimation().GetData().animationSpeed_;
		data_.animation.animationBlendTime = comboData.GetComboMotion().GetComboAnimation().GetData().animationBlendTime_;
		// アニメーション名前
		data_.animation.animationName = comboData.GetComboMotion().GetComboAnimation().GetData().animationName_;


		// 親
		data_.hitBox.parentName = comboData.GetComboHitBox().GetData().parentName;
		data_.hitBox.parentOffset = comboData.GetComboHitBox().GetData().offset;
		data_.hitBox.spawnType = comboData.GetComboHitBox().GetData().spawnType;
		data_.hitBox.dependenceType = comboData.GetComboHitBox().GetData().dependenceType;
		data_.hitBox.hitEffectType = comboData.GetComboHitBox().GetData().hitEffectType;
		data_.hitBox.lifetimeType = comboData.GetComboHitBox().GetData().lifetimeType;

		// 終了条件
		data_.endConditionType = comboData.GetComboCondition().GetEndCondition().GetData().type;

		// ロックオン
		data_.lockOn.lockOnRadius = comboData.GetComboMotion().GetComboMove().GetData().lockOnData_.radius;
		data_.lockOn.lockOnType = comboData.GetComboMotion().GetComboMove().GetData().lockOnData_.type;

		// シーケンサー適応
		ComboImGui::SequencerApplyToState(sequence_, comboData, maxFrame);
	}

#pragma endregion // ImGui管理

#pragma endregion // コンボ単位管理


#pragma region コンボエディター

	void Editor::Initialize(Engine::LineCommon* lineCommon, Combo::System* comboSystem, Engine::GlobalVariables* globalVariables, Character::BaseCharacter* owner) {
		this->comboSystem = comboSystem;
		this->lineCommon = lineCommon;
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
			Character::BasePlayer* player = dynamic_cast<Character::BasePlayer*>(owner);
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


		if (isComboEditorActive_)
			UpdateImGui(dt);

		ImGui::End();
#endif // _DEBUG
	};

	void Editor::UpdateImGui(float dt) {
#ifdef _DEBUG

		// 何もなければ何もしない
		if (comboEditorBlocks_.empty()) return;
		
		// 
		ComboImGui::Select("Selected Combo", selectedComboEditorBlockName_, comboEditorBlocks_);

		ImGui::Separator();
		ImGui::Text("Editing: %s", selectedComboEditorBlockName_.c_str());


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
			GlobalData& data = comboSystem->GetComboGlobalData(it.first);

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

			
			// リアクション
			data.reaction = comboEditorBlocks_[it.first].GetData().reaction;
			
			// ヒットボックス生成時間
			float hitBoxStart = ConvertUtility::FramesToSeconds(combo.GetEvent("ヒットボックス生成時間").startFrame);
			data.hitBox = comboEditorBlocks_[it.first].GetData().hitBox;
			data.hitBox.hitBoxWindowStart = hitBoxStart;
			data.hitBox.hitBoxLifeTime = ConvertUtility::FramesToSeconds(combo.GetEvent("ヒットボックス生成時間").endFrame) - hitBoxStart;

			// 移動時間
			data.move = comboEditorBlocks_[it.first].GetData().move;
			data.move.moveWindowStart = ConvertUtility::FramesToSeconds(combo.GetEvent("移動時間").startFrame);
			data.move.moveWindowEnd = ConvertUtility::FramesToSeconds(combo.GetEvent("移動時間").endFrame);
		
			
			// アニメーションスピード
			data.animation = comboEditorBlocks_[it.first].GetData().animation;
		
			// エフェクト
			data.trailEffectStartTime = ConvertUtility::FramesToSeconds(combo.GetEvent("トレイルエフェクト時間").startFrame);
			data.trailEffectLifeTime = ConvertUtility::FramesToSeconds(combo.GetEvent("トレイルエフェクト時間").endFrame) - data.trailEffectStartTime;

		
			// 終了条件
			data.endConditionType = comboEditorBlocks_[it.first].GetData().endConditionType;

			// ロックオン
			data.lockOn = comboEditorBlocks_[it.first].GetData().lockOn;
		}

		comboSystem->SetGlobalComboDatas();
	}

	void Editor::CreateComboEditorBlock(const std::string& comboName, Combo::System* comboSystem, std::shared_ptr<NodeState> state, Character::BaseCharacter* owner) {

		// 既に存在する場合は追加しない
		if (comboEditorBlocks_.find(comboName) != comboEditorBlocks_.end()) {
			return;
		}

		// コンボエディターブロック作成
		EditorBlock block;
		block.Initialize(lineCommon, globalVariables, comboSystem, state, owner);

		// 名前リストに追加
		comboEditorBlockNames_.push_back(comboName);

		// マップに追加
		comboEditorBlocks_[comboName] = block;
	}

#pragma endregion // コンボエディター

}

