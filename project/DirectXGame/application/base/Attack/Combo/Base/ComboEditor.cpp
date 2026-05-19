#include "ComboEditor.h"

#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"

namespace Combo {

#pragma region ComboEditorBlock

	void EditorBlock::Initialize(Engine::LineCommon* lineCommon,
		Engine::GlobalVariables* globalVariables, Combo::System* comboSystem,
		const std::string& stateName, Character::BaseCharacter* owner,
		EffectSystem* effectSystem) {
		this->globalVariables = globalVariables;	// 保存項目
		this->lineCommon = lineCommon;
		this->stateName = stateName;						// ステート
		this->owner = owner;						// オーナー
		this->comboSystem = comboSystem;			// コンボシステム
		this->effectSystem = effectSystem;

		// シーケンサー初期化
		sequence_.ClearEvents();

		// 最大フレーム設定
		sequence_.SetFrameMax(maxFrame);

		// コンボ名設定
		comboName_ = "Combo_" + stateName;

		// シーケンサーにコンボデータを適用させる
		SequencerApplyToState();
		// ステートのコンボデータ取得	
		ComboData& comboData = comboSystem->GetComboNodeState(stateName)->Data();
		// ステートの時間設定
		comboData.SetTimer(ConvertUtility::FramesToSeconds(currentFrame, 60.0f));
	}

	void EditorBlock::UpdateImGui(float dt, bool isActive) {

		auto state = comboSystem->GetComboNodeState(stateName);
		// 選択中でなければ処理しない
		if (!nowChoice_) return;
		if (currentFrame == 0 && isPlaying && isActive) {
			comboSystem->GetComboStateMachine()->SetRoot(state);
		}
		if (isActive)
			state->SetTimeInState(ConvertUtility::FramesToSeconds(currentFrame, 60.0f));

#ifdef _DEBUG

		ImGui::Begin("Attack Editor");
		ImGui::Separator();
		ImGui::Text("Combo Name: %s", comboName_.c_str());
		ImGui::Separator();

		// 現在の時間
		ComboImGui::CurrentFrame(dt, isActive, sequence_, isPlaying, loopPlay, currentFrame, firstFrame, maxFrame);

		// シーケンサーの設定と表示
		ComboImGui::SequenceSettings(sequence_, currentFrame, firstFrame, maxFrame, expanded, selected);

		// アニメーション
		ComboImGui::ApplyAnimationToState("アニメーション", isActive, currentFrame, maxFrame, data_.animation,
			owner->GetObjectComponent()->GetObject3D()->GetModel()->GetModelData().animations,
			owner->GetObjectComponent()->GetObject3D()->GetAnimationComponent(), state->GetAnimationName());


		// 終了条件
		ImGuiEndConditionType();
		// 移動関係設定
		ImGuiMove();
		// リアクション設定
		ImGuiReaction();
		// ヒットボックス設定
		ImGuiApplyHitBox();
		// カメラ設定
		ImGuiCamera();
		// ステートのコンボデータ取得	
		ComboData& comboData = state->Data();
		// ステートの時間設定
		comboData.SetTimer(ConvertUtility::FramesToSeconds(currentFrame, 60.0f));
		comboData.SetIsDebug(true);
		ImGui::End();

#endif // _DEBUG
	}

#pragma region ImGui

	void EditorBlock::ImGuiApplyHitBox() {
		if (ImGui::CollapsingHeader("ヒットボックス")) {

			// 形状タイプ
			static const char* HitBoxShapeTypeLabels[] = {
				"OBB",
				"AABB",
				"Sphere"
			};
			Engine::ImGuiManager::Select("形状", HitBoxShapeTypeLabels, data_.hitBox.shapeType);


			// 使用者タイプ
			static const char* HitBoxUseTypeLabels[] = {
				"プレイヤー",
				"敵",
				"その他",
			};
			Engine::ImGuiManager::Select("使用者", HitBoxUseTypeLabels, data_.hitBox.useType);


			// ペアレント設定
			Engine::ImGuiManager::Select("依存先", data_.hitBox.parentName, comboSystem->GetParentTransforms());

			// ヒットボックス出現条件
			static const char* HitBoxSpawnTypeLabels[] = {
				"時間経過",
				"着地",
				"空中",
				"ボタンを離したら",
			};
			Engine::ImGuiManager::Select("ヒットボックス出現条件", HitBoxSpawnTypeLabels, data_.hitBox.spawnType);

			// 依存先タイプ
			static const char* HitBoxParentTypeLabels[] = {
				"親子付け",
				"孤立",
				"追従先からの孤立",
				"ターゲットの位置",
			};
			Engine::ImGuiManager::Select("ヒットボックス依存先", HitBoxParentTypeLabels, data_.hitBox.dependenceType);

			// 影響タイプ
			static const char* HitBoxHitEffectTypeLabels[] = {
				"ダメージのみ",
				"力の影響のみ",
				"ダメージと力の影響",
			};
			Engine::ImGuiManager::Select("ヒットボックス影響", HitBoxHitEffectTypeLabels, data_.hitBox.hitEffectType);

			// 生存タイプ
			static const char* HitBoxLifetimeTypeLabels[] = {
				"無期限",
				"期限付き",
			};
			Engine::ImGuiManager::Select("ヒットボックス生存", HitBoxLifetimeTypeLabels, data_.hitBox.lifetimeType);


			// タグタイプ
			static const char* HitBoxTagTypeLabels[] = {
				"何もなし",
				"プレイヤー",
				"敵",
				"プレイヤー攻撃",
				"敵攻撃",
				"障害物(壁や床)",
				"アイテム",
				"プレイヤーによる影響",
				"敵による影響"
			};
			Engine::ImGuiManager::Select("コライダー(タグ)", HitBoxTagTypeLabels, data_.hitBox.tag);

			// レイヤタイプ
			static const char* HitBoxLayerTypeLabels[] = {
				"デフォルト",
				"プレイヤー",
				"敵",
				"プレイヤー攻撃",
				"敵攻撃",
				"環境",
				"全て",
			};
			Engine::ImGuiManager::Select("コライダー(レイヤー)", HitBoxLayerTypeLabels, data_.hitBox.layer);
			Engine::ImGuiManager::Select("コライダー(マスク)", HitBoxLayerTypeLabels, data_.hitBox.mask);


			// ヒット記録を使用
			ImGui::Checkbox("ヒット記録を使用", &data_.hitBox.useContactRecord);
			// オフセット
			ImGui::DragFloat3("オフセット", &data_.hitBox.offset.x, 0.1f);
			// サイズ
			if (data_.hitBox.shapeType == HitBox::ShapeType::kAABB ||
				data_.hitBox.shapeType == HitBox::ShapeType::kOBB)
				ImGui::DragFloat3("コライダーサイズ", &data_.hitBox.colliderSize.x, 0.1f);
			// 半径
			if (data_.hitBox.shapeType == HitBox::ShapeType::kSphere)
				ImGui::DragFloat("コライダー半径", &data_.hitBox.radius, 0.1f);

		}
	}

	void EditorBlock::ImGuiEndConditionType() {
		if (ImGui::CollapsingHeader("条件")) {
			static const char* EndConditionTypeLabels[] = {
			"着地したら",
			"離したら",
			"メータが空になったら",
			"時間経過",
			"当たったら",
			"特殊ケース",
			};
			Engine::ImGuiManager::Select("終了条件タイプ", EndConditionTypeLabels, data_.condition.endConditionType);

			ImGui::SliderFloat("入力遅延", &data_.condition.inputDelay, 0.0f, 1.0f, "%.2f");
			ImGui::Checkbox("強制的に移行", &data_.condition.isCompulsionNext);
			ImGui::Checkbox("キャンセル可能", &data_.condition.isCancel);
			ImGui::Checkbox("移動キャンセル可能", &data_.condition.isMoveCancel);
		}
	}

	void EditorBlock::ImGuiMove() {
		if (ImGui::CollapsingHeader("移動関係")) {
			static const char* MoveTypeLabels[] = {
			"入力方向",
			"ターゲットに向かって",
			"前方",
			"カメラ方向",
			};
			Engine::ImGuiManager::Select("移動タイプ", MoveTypeLabels, data_.move.moveType);

			ImGui::Checkbox("強制移動", &data_.move.isCompulsionMove);
			ImGui::SliderFloat3("移動速度", &data_.move.moveSpeed.x, 0.0f, 1000.0f, "%.2f");

			ImGui::DragFloat3("ローカル移動ベクトル", &data_.move.localMoveVector.x, 0.01f);
			if (data_.move.localMoveVector.x >= 1.0f) {
				data_.move.localMoveVector.x = 1.0f;
			}
			if (data_.move.localMoveVector.y >= 1.0f) {
				data_.move.localMoveVector.y = 1.0f;
			}
			if (data_.move.localMoveVector.z >= 1.0f) {
				data_.move.localMoveVector.z = 1.0f;
			}
			if (data_.move.localMoveVector.x <= -1.0f) {
				data_.move.localMoveVector.x = -1.0f;
			}
			if (data_.move.localMoveVector.y <= -1.0f) {
				data_.move.localMoveVector.y = -1.0f;
			}
			if (data_.move.localMoveVector.z <= -1.0f) {
				data_.move.localMoveVector.z = -1.0f;
			}
			ImGui::Checkbox("ローカル移動ベクトルを正規化してから使うか", &data_.move.isNormalizeLocalMove);
			ImGui::Checkbox("移動中も毎フレーム方向を更新するか", &data_.move.isUpdateDirectionEachFrame);
			ImGui::Checkbox("ターゲット方向を使うとき、基準前方を水平化するか", &data_.move.isFlattenTargetDirection);
			ImGui::Checkbox("移動方向とキャラクターの向く方向を一致させるか", &data_.move.alignCharacterToMovement);

			ImGui::Checkbox("重力", &data_.move.isGravity);
			ImGui::SliderFloat("重力倍率", &data_.move.gravityScale, 0.0f, 100.0f, "%.2f");
			ImGui::Checkbox("開始重力速度をリセット", &data_.move.isResetGravity);

			// ターゲットなら
			if (data_.move.moveType == Combo::MoveType::kTraget) {
				static const char* LockOnTypeLabels[] = {
				"当てた相手",
				"近い相手",
				};
				Engine::ImGuiManager::Select("ロックオンタイプ", LockOnTypeLabels, data_.move.lockOnData.type);
				ImGui::SliderFloat("ソフトロックオン半径", &data_.move.lockOnData.radius, 0.0f, 100.0f);
				static const char* TargetMoveTypeLabels[] = {
				"なし",
				"移動",
				"瞬間移動",
				"補間移動"
				};
				Engine::ImGuiManager::Select("ターゲット移動タイプ", TargetMoveTypeLabels, data_.move.lockOnData.targetMoveType);
				ImGui::SliderFloat("ターゲット接近距離半径", &data_.move.lockOnData.moveTargetRadius, 0.0f, 100.0f);

			}
		}
	}

	// リアクション設定
	void EditorBlock::ImGuiReaction() {
		if (ImGui::CollapsingHeader("リアクション")) {
			static const char* HitReactionTypeLabels[] = {
				"ノックバック",
				"吹っ飛び",
				"打ち上げ",
				"壁バウンド"
			};
			Engine::ImGuiManager::Select("ヒットリアクションタイプ", HitReactionTypeLabels, data_.hitReaction.type);

			ImGui::SliderFloat("持続時間", &data_.hitReaction.duration, 0.0f, 5.0f, "%.2f");
			ImGui::SliderFloat("パワー", &data_.hitReaction.power, 0.0f, 999.0f, "%.2f");
			ImGui::SliderFloat("縦方向パワー", &data_.hitReaction.verticalBoost, 0.0f, 999.0f, "%.2f");
			ImGui::Checkbox("縦方向移動", &data_.hitReaction.isVerticalBoost);
			float damage = data_.hitReaction.damageData.GetOne().GetDamage();
			ImGui::SliderFloat("ダメージ", &damage, 0.0f, 1000.0f, "%.2f");
			data_.hitReaction.damageData.GetOne().SetDamage(damage);
			ImGui::Checkbox("一回しかヒットストップしない", &data_.hitReaction.isSingleHitStop);

			ImGui::SliderFloat("ヒットストップ時間(相手)", &data_.hitReaction.targetHitStopTime, 0.0f, 10.0f, "%.2f");
			ImGui::SliderFloat("ヒットストップ時間(自分)", &data_.hitReaction.selfHitStopTime, 0.0f, 10.0f, "%.2f");

			ImGui::SliderFloat("ヒットスタン持続時間", &data_.hitReaction.hitStunTime, 0.0f, 100.0f, "%.2f");
			ImGui::SliderFloat("ダウン持続時間", &data_.hitReaction.downTime, 0.0f, 100.0f, "%.2f");
			ImGui::SliderFloat("打ち上げ持続時間", &data_.hitReaction.launchFloatTime, 0.0f, 100.0f, "%.2f");
			ImGui::Checkbox("ヒット重力", &data_.hitReaction.gravityEnabled);
			ImGui::SliderFloat("重力倍率", &data_.hitReaction.gravityScale, 0.0f, 100.0f, "%.2f");

			DrawHitEffectEditor(data_.hitReaction, effectSystem->GetEffectGlobalDatas());
		}
	}

	// カメラ設定
	void  EditorBlock::ImGuiCamera() {
		if (ImGui::CollapsingHeader("カメラ関係")) {
			ImGui::Checkbox("カメラを対象にロックオンするか", &data_.camera.isLockOn);
			ImGui::Checkbox("カメラロックオンの回転引き継ぎ", &data_.camera.isLockOnRotate);
			ImGui::DragFloat("ロックオン補間速度", &data_.camera.lockOnInterpolation,0.01f, 0.0f, 10.0f);
			
			ImGui::Checkbox("カメラをズームするか", &data_.camera.isZoom);
			ImGui::Checkbox("カメラがロックオンしたときだけズームするか", &data_.camera.isLockOnZoom);
			ImGui::SliderFloat("カメラズーム速度", &data_.camera.zoomSpeed, 0.01f, 100.0f, "%.2f");
			ImGui::DragFloat("カメラズーム時間", &data_.camera.zoomDuration,0.01f ,0.0f, 60.0f);
			ImGui::DragFloat("カメラズーム開始時間", &data_.camera.zoomStartTime, 0.01f, 0.0f, 60.0f);
			ImGui::DragFloat("カメラズーム補間量", &data_.camera.zoomTargetDistance, 0.01f, 0.01f, 10.0f);

		}
	};

	void EditorBlock::SequencerApplyToState() {

		// ステートのコンボデータ取得	
		ComboData& comboData = comboSystem->GetComboNodeState(stateName)->Data();

		// 移動データ
		data_.move = comboData.GetComboMotion().GetComboMove().GetData();
		// ヒットボックスデータ
		data_.hitBox = comboData.GetComboHitBox().GetCollData().hitBoxData;
		// リアクションデータ
		data_.hitReaction = comboData.GetComboHitBox().GetCollData().reactionData;
		// アニメーション
		data_.animation = comboData.GetComboMotion().GetComboAnimation().GetData();
		// 条件
		data_.condition = comboData.GetComboCondition().GetData();
		// カメラ
		data_.camera = comboData.GetComboCamera().GetData();
		// エフェクト
		data_.effect = comboData.GetComboEffect().GetData();
		// シーケンサー適応
		ComboImGui::SequencerApplyToState(sequence_, comboData, maxFrame);
	}

	void EditorBlock::DrawHitEffectEditor(HitReactionData& reaction,
		const std::map<std::string, EffectGlobalData>& effectDatas) {
		ImGui::SeparatorText("ヒットエフェクト");
		ImGui::Text("※エディタでリロードしてから保存してください");

		int removeIndex = -1;

		for (int i = 0; i < static_cast<int>(reaction.hitEffectNames.size()); ++i) {
			ImGui::PushID(i);

			HitEffectEntry& entry = reaction.hitEffectNames[i];

			char slotBuffer[128]{};
			char effectBuffer[128]{};

			strncpy_s(slotBuffer, entry.slotName.c_str(), _TRUNCATE);
			strncpy_s(effectBuffer, entry.effectName.c_str(), _TRUNCATE);

			ImGui::InputText("スロット名", slotBuffer, sizeof(slotBuffer));
			ImGui::InputText("エフェクト名", effectBuffer, sizeof(effectBuffer));

			entry.slotName = slotBuffer;
			entry.effectName = effectBuffer;

			if (ImGui::BeginCombo("エフェクト一覧", entry.effectName.c_str())) {
				for (const auto& effectPair : effectDatas) {
					const bool isSelected = (entry.effectName == effectPair.first);
					if (ImGui::Selectable(effectPair.first.c_str(), isSelected)) {
						entry.effectName = effectPair.first;
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("削除")) {
				removeIndex = i;
			}

			ImGui::Separator();
			ImGui::PopID();
		}

		if (removeIndex >= 0 && removeIndex < static_cast<int>(reaction.hitEffectNames.size())) {
			reaction.hitEffectNames.erase(reaction.hitEffectNames.begin() + removeIndex);
		}

		if (ImGui::Button("ヒットエフェクト追加")) {
			reaction.hitEffectNames.push_back(HitEffectEntry{
				.slotName = "NewEffect",
				.effectName = ""
				});
		}
	}

#pragma endregion // ImGui管理

#pragma endregion // コンボ単位管理


#pragma region コンボエディター

	void Editor::Initialize(Engine::LineCommon* lineCommon,
		Combo::System* comboSystem, Engine::GlobalVariables* globalVariables,
		Character::BaseCharacter* owner, EffectSystem* effectSystem) {
		this->comboSystem = comboSystem;
		this->lineCommon = lineCommon;
		this->globalVariables = globalVariables;
		this->owner = owner;
		this->effectSystem = effectSystem;

		// コンボシステムからコンボエディターブロックを作成
		ApplyComboEditorToSystem();
	}

	void Editor::Update(float dt) {
#ifdef _DEBUG
		ImGui::Begin("Comdo");
		ImGui::Checkbox("isCreativeMode", &isComboEditorActive_);
		comboSystem->SertIsDebug(isComboEditorActive_);
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

		ImGui::Separator();
		if (ImGui::Button("SaveComboName")) {
			globalVariables->SaveFile(comboSystem->GetName());
		}
		ImGui::Separator();


		// 全てセーブ
		if (ImGui::Button("AllSave")) {
			for (auto& it : comboSystem->GetComboNodeStates()) {
				globalVariables->SaveFile(it.first);
			}
		}
		// セーブ
		if (ImGui::Button("Save")) {
			for (auto& it : comboSystem->GetComboNodeStates()) {
				if (it.first == selectedComboEditorBlockName_)
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

		// コンボシステムからコンボノードステートを取得してコンボエディターブロックを作成
		for (auto& comboState : comboSystem->GetComboNodeStates()) {
			CreateComboEditorBlock(comboState.second->GetName(), comboSystem, comboState.first, owner);
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
			data.effect.trailEffectStartTime = ConvertUtility::FramesToSeconds(combo.GetEvent("トレイルエフェクト時間").startFrame);
			data.effect.trailEffectLifeTime = ConvertUtility::FramesToSeconds(combo.GetEvent("トレイルエフェクト時間").endFrame) - data.effect.trailEffectStartTime;



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

