#include "ComboEditorBlock.h"


#include "DirectXGame/application/base/Character/Base/BaseCharacter.h"
#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"
#include "DirectXGame/application/base/Effect/Effect.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include <cctype>
#include <cstring>

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
		// 攻撃種別と遠距離設定
		ImGuiAttackType();
		// 遠距離設定
		ImGuiRange();
		// 演出設定
		ImGuiEffect();
		// 移動関係設定
		ImGuiMove();
		// リアクション設定
		ImGuiReaction();
		// ヒットボックス設定
		ImGuiApplyHitBox();
		// カメラ設定
		ImGuiCamera();
		// コンボ接続設定
		ImGuiConnection();
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
			ImGui::Checkbox("コライダー別にヒット記録", &data_.hitBox.recordPerCollider);
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

			ImGui::SliderFloat("入力バッファ時間", &data_.condition.inputBufferTime, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("入力遅延", &data_.condition.inputDelay, 0.0f, 1.0f, "%.2f");
			ImGui::Checkbox("強制的に移行", &data_.condition.isCompulsionNext);
			ImGui::Checkbox("キャンセル可能", &data_.condition.isCancel);
			ImGui::Checkbox("移動キャンセル可能", &data_.condition.isMoveCancel);

			ImGui::SeparatorText("キャンセル制約");
			ImGui::Checkbox("ヒット時のみキャンセル", &data_.action.cancelOnHitOnly);
			ImGui::Checkbox("ミス時のみキャンセル", &data_.action.cancelOnMissOnly);
			ImGui::Checkbox("着地時のみキャンセル", &data_.action.landingCancel);
		}
	}

	void EditorBlock::ImGuiAttackType() {
		if (!ImGui::CollapsingHeader("攻撃種別")) {
			return;
		}

		static const char* ComboTypeLabels[] = {
			"移動のみ",
			"近距離",
			"遠距離",
			"近距離 + 遠距離",
		};
		Engine::ImGuiManager::Select("攻撃タイプ", ComboTypeLabels, data_.type);

		ImGui::SeparatorText("攻撃ノード設定");
		ImGui::Checkbox("スタミナコスト個別指定", &data_.action.useCustomStaminaCost);
		ImGui::DragFloat("スタミナコスト", &data_.action.staminaCost, 0.1f, 0.0f, 999.0f, "%.2f");
		ImGui::DragFloat("クールダウン", &data_.action.cooldown, 0.01f, 0.0f, 60.0f, "%.2f");
		ImGui::DragInt("空中残り回数要求", &data_.action.requiredAirRemainCount, 1, 0, 10);
		ImGui::Checkbox("スーパーアーマー", &data_.action.superArmor);
		ImGui::Checkbox("無敵", &data_.action.invincible);
		ImGui::Checkbox("ガードポイント", &data_.action.guardPoint);
		ImGui::DragFloat("ヒットポーズ倍率", &data_.action.hitPauseScale, 0.01f, 0.0f, 5.0f, "%.2f");
		ImGui::DragFloat("カメラシェイク量", &data_.action.cameraShakePower, 0.01f, 0.0f, 10.0f, "%.2f");
	}

	void EditorBlock::ImGuiRange() {
		if (data_.type != Type::kRange && data_.type != Type::kMix) {
			return;
		}
		if (!ImGui::CollapsingHeader("遠距離設定")) {
			return;
		}

		static const char* RangeTypeLabels[] = {
			"弾",
			"武器",
			"サブ武器",
		};
		Engine::ImGuiManager::Select("遠距離タイプ", RangeTypeLabels, data_.range.rangeType);

		ImGui::SeparatorText("共通の狙い設定");
		static const char* RangeLockOnTypeLabels[] = {
			"前方",
			"ターゲット",
			"カメラ",
			"オフセットターゲット",
		};
		Engine::ImGuiManager::Select("狙いタイプ", RangeLockOnTypeLabels, data_.range.lockOnType);
		if (data_.range.lockOnType == RangeLockOnType::kTarget) {
			ImGui::DragFloat("狙い始める半径", &data_.range.lockOnStartRadius, 0.1f, 0.0f, 1000.0f, "%.2f");
		}
		if (data_.range.lockOnType == RangeLockOnType::kOffsetTarget) {
			static const char* RangeOffsetTargetTypeLabels[] = {
				"ワールドオフセット",
				"所有者ローカル",
			};
			Engine::ImGuiManager::Select("オフセットタイプ", RangeOffsetTargetTypeLabels, data_.range.offsetTargetType);
			ImGui::DragFloat3("オフセットターゲット", &data_.range.offsetTarget.x, 0.1f);
		}

		ImGui::SeparatorText("共通の使用タイミング");
		ImGui::DragFloat("開始時間", &data_.range.rangeWindowStart, 0.01f, 0.0f, 60.0f, "%.2f");
		ImGui::DragFloat("終了時間", &data_.range.rangeWindowEnd, 0.01f, 0.0f, 60.0f, "%.2f");
		ImGui::DragFloat("使用間隔", &data_.range.interval, 0.01f, 0.001f, 60.0f, "%.3f");
		ImGui::DragInt("使用数", &data_.range.count, 1.0f, 1, 100);

		if (data_.range.rangeType == RangeType::kBullet) {
			ImGui::SeparatorText("弾設定");
			ImGui::DragFloat("弾速", &data_.range.speed, 0.1f, 0.0f, 1000.0f, "%.2f");
			ImGui::DragFloat("弾ダメージ", &data_.range.damage, 0.1f, 0.0f, 1000.0f, "%.2f");
		}

		if (data_.range.rangeType == RangeType::kSubWeapon) {
			ImGui::SeparatorText("投擲物設定");
			static const char* RangeThrowMoveTypeLabels[] = {
				"直進",
				"ヒット時停止",
				"ターゲット瞬間移動",
				"ターゲット補間移動",
			};
			Engine::ImGuiManager::Select("投擲移動タイプ", RangeThrowMoveTypeLabels, data_.range.throwMoveType);
			if (data_.range.throwMoveType == RangeThrowMoveType::kLerpToTarget) {
				ImGui::DragFloat("投擲補間時間", &data_.range.throwLerpTime, 0.01f, 0.001f, 60.0f, "%.3f");
			}

			static const char* RangeEffectTriggerTypeLabels[] = {
				"なし",
				"ボタン",
				"時間経過",
				"ヒット",
			};
			Engine::ImGuiManager::Select("効果発動条件", RangeEffectTriggerTypeLabels, data_.range.effectTriggerType);
			if (data_.range.effectTriggerType == RangeEffectTriggerType::kTimer) {
				ImGui::DragFloat("効果発動時間", &data_.range.effectTriggerTime, 0.01f, 0.0f, 60.0f, "%.2f");
			}

			static const char* RangeRecallTriggerTypeLabels[] = {
				"なし",
				"ボタン",
				"時間経過",
				"近づく",
			};
			Engine::ImGuiManager::Select("回収条件", RangeRecallTriggerTypeLabels, data_.range.recallTriggerType);
			if (data_.range.recallTriggerType == RangeRecallTriggerType::kTimer) {
				ImGui::DragFloat("回収時間", &data_.range.recallTriggerTime, 0.01f, 0.0f, 60.0f, "%.2f");
			}
			if (data_.range.recallTriggerType == RangeRecallTriggerType::kNearOwner) {
				ImGui::DragFloat("回収近接半径", &data_.range.recallNearRadius, 0.1f, 0.0f, 1000.0f, "%.2f");
			}
			ImGui::DragFloat("回収前の停滞時間", &data_.range.throwStayTime, 0.01f, 0.0f, 60.0f, "%.2f");

			ImGui::SeparatorText("サブウェポン表示設定");
			ImGui::DragFloat3("待機位置", &data_.range.subWeaponIdleOffset.x, 0.01f);
			ImGui::DragFloat3("投擲開始オフセット", &data_.range.subWeaponStartOffset.x, 0.01f);
			ImGui::DragFloat("投擲速度", &data_.range.subWeaponThrowSpeed, 0.1f, 0.0f, 1000.0f, "%.2f");
			ImGui::DragFloat("投擲時間", &data_.range.subWeaponThrowLifeTime, 0.01f, 0.001f, 60.0f, "%.3f");
			ImGui::DragFloat("戻り時間", &data_.range.subWeaponReturnTime, 0.01f, 0.001f, 60.0f, "%.3f");
			ImGui::DragFloat("回転速度", &data_.range.subWeaponSpinSpeed, 0.1f, 0.0f, 1000.0f, "%.2f");
			ImGui::Checkbox("投擲方向に向ける", &data_.range.subWeaponAlignToDirection);
			ImGui::Checkbox("スピン", &data_.range.subWeaponUseSpin);
			ImGui::DragFloat3("回転オフセット", &data_.range.subWeaponRotateOffset.x, 0.01f);
		}

		if (data_.range.rangeWindowEnd < data_.range.rangeWindowStart) {
			data_.range.rangeWindowEnd = data_.range.rangeWindowStart;
		}
		if (data_.range.interval < 0.001f) {
			data_.range.interval = 0.001f;
		}
		if (data_.range.count < 1) {
			data_.range.count = 1;
		}
		if (data_.range.subWeaponThrowLifeTime < 0.001f) {
			data_.range.subWeaponThrowLifeTime = 0.001f;
		}
		if (data_.range.subWeaponReturnTime < 0.001f) {
			data_.range.subWeaponReturnTime = 0.001f;
		}
		if (data_.range.throwLerpTime < 0.001f) {
			data_.range.throwLerpTime = 0.001f;
		}
		if (data_.range.lockOnStartRadius < 0.0f) {
			data_.range.lockOnStartRadius = 0.0f;
		}
		if (data_.range.recallNearRadius < 0.0f) {
			data_.range.recallNearRadius = 0.0f;
		}
		if (data_.range.throwStayTime < 0.0f) {
			data_.range.throwStayTime = 0.0f;
		}
	}

	void EditorBlock::ImGuiEffect() {
		if (!ImGui::CollapsingHeader("演出")) {
			return;
		}

		// 武器の表示有無はコンボ演出側で制御する
		ImGui::Checkbox("武器表示", &data_.effect.weaponDraw);
		ImGui::TextDisabled("トレイル時間はシーケンサーで調整します。");
		if (effectSystem) {
			DrawComboEffectEditor(data_.effect, effectSystem->GetEffectGlobalDatas());
		}
	}

	void EditorBlock::ImGuiMove() {
		if (ImGui::CollapsingHeader("移動関係")) {
			auto clampLocalMoveVector = [](Vector3& localMoveVector) {
				if (localMoveVector.x >= 1.0f) {
					localMoveVector.x = 1.0f;
				}
				if (localMoveVector.y >= 1.0f) {
					localMoveVector.y = 1.0f;
				}
				if (localMoveVector.z >= 1.0f) {
					localMoveVector.z = 1.0f;
				}
				if (localMoveVector.x <= -1.0f) {
					localMoveVector.x = -1.0f;
				}
				if (localMoveVector.y <= -1.0f) {
					localMoveVector.y = -1.0f;
				}
				if (localMoveVector.z <= -1.0f) {
					localMoveVector.z = -1.0f;
				}
				};
			auto drawConditionalMove = [&](const char* label, GlobalMoveTargetParameters& parameters, bool drawTargetSettings) {
				ImGui::PushID(label);
				ImGui::SeparatorText(label);
				ImGui::Checkbox("この条件の移動を上書き", &parameters.enabled);
				if (parameters.enabled) {
					if (ImGui::Button("基本移動をコピー")) {
						parameters.moveSpeed = data_.move.moveSpeed;
						parameters.localMoveVector = data_.move.localMoveVector;
						parameters.isNormalizeLocalMove = data_.move.isNormalizeLocalMove;
						parameters.targetMoveType = data_.move.lockOnData.targetMoveType;
						parameters.moveTargetRadius = data_.move.lockOnData.moveTargetRadius;
					}
					ImGui::DragFloat3("移動速度", &parameters.moveSpeed.x, 0.1f);
					ImGui::DragFloat3("ローカル移動ベクトル", &parameters.localMoveVector.x, 0.01f);
					clampLocalMoveVector(parameters.localMoveVector);
					ImGui::Checkbox("ローカル移動ベクトルを正規化してから使うか", &parameters.isNormalizeLocalMove);
					if (drawTargetSettings) {
						static const char* TargetMoveTypeLabels[] = {
						"なし",
						"移動",
						"瞬間移動",
						"補間移動"
						};
						Engine::ImGuiManager::Select("ターゲット移動タイプ", TargetMoveTypeLabels, parameters.targetMoveType);
						ImGui::SliderFloat("ターゲット接近距離半径", &parameters.moveTargetRadius, 0.0f, 100.0f);
					}
				}
				ImGui::PopID();
				};
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
			clampLocalMoveVector(data_.move.localMoveVector);
			ImGui::Checkbox("ローカル移動ベクトルを正規化してから使うか", &data_.move.isNormalizeLocalMove);
			ImGui::Checkbox("移動中も毎フレーム方向を更新するか", &data_.move.isUpdateDirectionEachFrame);
			ImGui::Checkbox("移動中も毎フレームターゲット位置を更新するか", &data_.move.isUpdateTargetPositionEachFrame);
			static const char* MoveSpeedCurveTypeLabels[] = {
			"一定",
			"加速",
			"減速",
			"加速して減速",
			};
			Engine::ImGuiManager::Select("速度カーブ", MoveSpeedCurveTypeLabels, data_.move.speedCurveType);
			ImGui::DragFloat("速度カーブ強度", &data_.move.speedCurvePower, 0.01f, 0.001f, 10.0f);
			ImGui::Checkbox("縦方向の攻撃移動を許可するか", &data_.move.isVerticalMove);
			ImGui::Checkbox("ターゲット方向を使うとき、基準前方を水平化するか", &data_.move.isFlattenTargetDirection);
			ImGui::Checkbox("移動方向とキャラクターの向く方向を一致させるか", &data_.move.alignCharacterToMovement);

			ImGui::Checkbox("重力", &data_.move.isGravity);
			ImGui::SliderFloat("重力倍率", &data_.move.gravityScale, 0.0f, 100.0f, "%.2f");
			ImGui::DragFloat("最大落下速度(0で無制限)", &data_.move.maxFallSpeed, 0.1f, 0.0f, 1000.0f, "%.2f");
			ImGui::Checkbox("開始重力速度をリセット", &data_.move.isResetGravity);

			// ターゲットなら
			if (data_.move.moveType == Combo::MoveType::kTraget) {
				static const char* LockOnTypeLabels[] = {
				"当てた相手",
				"近い相手",
				};
				Engine::ImGuiManager::Select("ロックオンタイプ", LockOnTypeLabels, data_.move.lockOnData.type);
				ImGui::SliderFloat("ソフトロックオン半径", &data_.move.lockOnData.softLockRadius, 0.0f, 100.0f);
				ImGui::SliderFloat("手動ロックオン半径", &data_.move.lockOnData.lockOnRadius, 0.0f, 100.0f);
				static const char* TargetMoveTypeLabels[] = {
				"なし",
				"移動",
				"瞬間移動",
				"補間移動"
				};
				Engine::ImGuiManager::Select("ターゲット移動タイプ", TargetMoveTypeLabels, data_.move.lockOnData.targetMoveType);
				ImGui::SliderFloat("ターゲット接近距離半径", &data_.move.lockOnData.moveTargetRadius, 0.0f, 100.0f);

			}

			ImGui::SeparatorText("ターゲット有無別の移動");
			ImGui::TextWrapped("上書きをオンにした条件だけ、基本移動の速度や方向を差し替えます。");
			drawConditionalMove("ターゲットあり", data_.move.targetMove, true);
			drawConditionalMove("ターゲットなし", data_.move.noTargetMove, false);
		}
	}

	// リアクション設定
	void EditorBlock::ImGuiReaction() {
		if (ImGui::CollapsingHeader("リアクション")) {
			static const char* HitReactionTypeLabels[] = {
				"ノックバック",
				"吹っ飛び",
				"打ち上げ",
				"壁バウンド",
				"吸い付き"
			};
			Engine::ImGuiManager::Select("ヒットリアクションタイプ", HitReactionTypeLabels, data_.hitReaction.type);

			ImGui::SliderFloat("持続時間", &data_.hitReaction.duration, 0.0f, 5.0f, "%.2f");
			ImGui::SliderFloat("パワー", &data_.hitReaction.power, 0.0f, 999.0f, "%.2f");
			ImGui::SliderFloat("縦方向パワー", &data_.hitReaction.verticalBoost, 0.0f, 999.0f, "%.2f");
			ImGui::Checkbox("縦方向移動", &data_.hitReaction.isVerticalBoost);
			float damage = data_.hitReaction.damageData.GetOne().GetDamage();
			ImGui::SliderFloat("ダメージ", &damage, 0.0f, 1000.0f, "%.2f");
			data_.hitReaction.damageData.GetOne().SetDamage(damage);

			static const char* SelfHitStopPolicyLabels[] = {
				"なし",
				"最初のヒットのみ",
				"毎ヒット",
			};
			Engine::ImGuiManager::Select("自分側ヒットストップ方針", SelfHitStopPolicyLabels, data_.hitReaction.selfHitStopPolicy);

			static const char* AttackAttributeLabels[] = {
				"なし",
				"物理",
				"炎",
				"氷",
				"雷",
				"特殊",
			};
			Engine::ImGuiManager::Select("攻撃属性", AttackAttributeLabels, data_.hitReaction.attribute);
			ImGui::DragInt("ヒット優先度", &data_.hitReaction.hitPriority, 1, -100, 100);
			ImGui::DragFloat("ヒットカメラシェイク量", &data_.hitReaction.cameraShakePower, 0.01f, 0.0f, 10.0f, "%.2f");

			ImGui::SliderFloat("ヒットストップ時間(相手)", &data_.hitReaction.targetHitStopTime, 0.0f, 10.0f, "%.2f");
			ImGui::SliderFloat("ヒットストップ時間(自分)", &data_.hitReaction.selfHitStopTime, 0.0f, 10.0f, "%.2f");

			ImGui::SliderFloat("ヒットスタン持続時間", &data_.hitReaction.hitStunTime, 0.0f, 100.0f, "%.2f");
			ImGui::SliderFloat("ダウン持続時間", &data_.hitReaction.downTime, 0.0f, 100.0f, "%.2f");
			ImGui::SliderFloat("打ち上げ持続時間", &data_.hitReaction.launchFloatTime, 0.0f, 100.0f, "%.2f");
			if (data_.hitReaction.type == HitReactionType::Launch) {
				ImGui::SliderFloat("打ち上げ最大高度(命中位置基準)", &data_.hitReaction.launchMaxHeight, 0.0f, 100.0f, "%.2f");
			}
			ImGui::Checkbox("ヒット重力", &data_.hitReaction.gravityEnabled);
			ImGui::SliderFloat("重力倍率", &data_.hitReaction.gravityScale, 0.0f, 100.0f, "%.2f");

			DrawHitEffectEditor(data_.hitReaction, effectSystem->GetEffectGlobalDatas());
		}
	}

	// カメラ設定
	void  EditorBlock::ImGuiCamera() {
		if (ImGui::CollapsingHeader("カメラ関係")) {
			if (ImGui::TreeNode("カメラ切り替え")) {
				ImGui::Checkbox("攻撃中にカメラを切り替える", &data_.camera.isChangeCamera);
				char cameraNameBuffer[128]{};
				strncpy_s(cameraNameBuffer, data_.camera.cameraName.c_str(), sizeof(cameraNameBuffer) - 1);
				if (ImGui::InputText("切り替え先カメラ名", cameraNameBuffer, sizeof(cameraNameBuffer))) {
					// CameraManagerに登録されている名前を保存する
					data_.camera.cameraName = cameraNameBuffer;
				}
				ImGui::DragFloat("カメラ切り替え開始時間", &data_.camera.changeCameraStartTime, 0.01f, 0.0f, 60.0f);
				ImGui::DragFloat("カメラ切り替え補間時間", &data_.camera.interpolation, 0.01f, 0.0f, 10.0f);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("ロックオン演出")) {
				ImGui::Checkbox("カメラを対象にロックオンするか", &data_.camera.isLockOn);
				ImGui::Checkbox("カメラロックオンの回転引き継ぎ", &data_.camera.isLockOnRotate);
				ImGui::DragFloat("ロックオン補間速度", &data_.camera.lockOnInterpolation, 0.01f, 0.0f, 10.0f);
				ImGui::DragFloat("ロックオン解除時間(0以下で解除しない)", &data_.camera.lockOnEndTime, 0.01f, 0.0f, 60.0f);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("ズーム演出")) {
				ImGui::Checkbox("カメラをズームするか", &data_.camera.isZoom);
				ImGui::Checkbox("カメラがロックオンしたときだけズームするか", &data_.camera.isLockOnZoom);
				ImGui::SliderFloat("カメラズーム速度", &data_.camera.zoomSpeed, 0.01f, 100.0f, "%.2f");
				ImGui::DragFloat("カメラズーム時間", &data_.camera.zoomDuration, 0.01f, 0.0f, 60.0f);
				ImGui::DragFloat("カメラズーム開始時間", &data_.camera.zoomStartTime, 0.01f, 0.0f, 60.0f);
				ImGui::DragFloat("カメラズーム補間量", &data_.camera.zoomTargetDistance, 0.01f, 0.01f, 10.0f);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("シェイク演出")) {
				ImGui::Checkbox("カメラをシェイクするか", &data_.camera.isShake);
				ImGui::DragFloat("カメラシェイク開始時間", &data_.camera.shakeStartTime, 0.01f, 0.0f, 60.0f);
				ImGui::DragFloat("カメラシェイク時間", &data_.camera.shakeDuration, 0.01f, 0.0f, 60.0f);
				ImGui::DragFloat("カメラシェイク量", &data_.camera.shakeCameraPower, 0.01f, 0.0f, 10.0f);
				ImGui::DragFloat3("カメラシェイク幅", &data_.camera.shakeOffset.x, 0.01f, 0.0f, 10.0f);
				ImGui::TreePop();
			}

		}
	};

	void EditorBlock::ImGuiConnection() {
		if (!ImGui::CollapsingHeader("コンボ接続")) {
			return;
		}

		auto drawConnection = [this](const char* label, std::string& target) {
			const char* preview = target.empty() ? "なし" : target.c_str();
			if (ImGui::BeginCombo(label, preview)) {
				const bool noSelection = target.empty();
				if (ImGui::Selectable("なし", noSelection)) {
					target.clear();
				}
				if (noSelection) {
					ImGui::SetItemDefaultFocus();
				}
				for (const std::string& comboName : conectComboNames_) {
					const bool selected = target == comboName;
					if (ImGui::Selectable(comboName.c_str(), selected)) {
						target = comboName;
					}
					if (selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			};

		auto drawInputConnections = [&](const char* inputLabel, std::string& defaultTarget,
			GlobalConditionalConnection& conditionalTargets) {
				ImGui::PushID(inputLabel);
				if (ImGui::TreeNode(inputLabel)) {
					drawConnection("標準の接続先", defaultTarget);
					drawConnection("地上 / 未ヒット", conditionalTargets.groundMiss);
					drawConnection("地上 / ヒット", conditionalTargets.groundHit);
					drawConnection("空中 / 未ヒット", conditionalTargets.airMiss);
					drawConnection("空中 / ヒット", conditionalTargets.airHit);
					drawConnection("ロックオン中", conditionalTargets.lockOn);
					drawConnection("ロックオンなし", conditionalTargets.noLockOn);
					ImGui::TreePop();
				}
				ImGui::PopID();
			};

		ImGui::TextWrapped("条件付き接続が未設定の場合は、標準の接続先が使用されます。");
		drawInputConnections("弱攻撃入力", data_.connection.lightAttack, data_.connection.lightCondition);
		drawInputConnections("強攻撃入力", data_.connection.heavyAttack, data_.connection.heavyCondition);
		drawInputConnections("スキル入力", data_.connection.skill, data_.connection.skillCondition);
	}

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
		// 攻撃ノード単位の調整項目
		data_.action = comboData.GetActionData();
		// カメラ
		data_.camera = comboData.GetComboCamera().GetData();
		// エフェクト
		data_.effect = comboData.GetComboEffect().GetData();
		// 接続
		data_.connection = comboSystem->GetComboGlobalData(stateName).connection;
		// 攻撃タイプと遠距離攻撃
		data_.type = comboData.GetType();
		data_.range = comboData.GetComboRange().GetData();
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

	void EditorBlock::DrawComboEffectEditor(GloblEffectData& effect,
		const std::map<std::string, EffectGlobalData>& effectDatas) {
		ImGui::SeparatorText("コンボエフェクト");
		ImGui::TextWrapped("条件に応じて、追従先位置 + オフセットへコンボエフェクトを発生させます。");

		int removeIndex = -1;
		bool shouldSyncSequence = false;

		for (int i = 0; i < static_cast<int>(effect.comboEffects.size()); ++i) {
			ImGui::PushID(i);

			ComboEffectEntry& entry = effect.comboEffects[i];
			char effectBuffer[128]{};
			strncpy_s(effectBuffer, entry.effectName.c_str(), _TRUNCATE);

			ImGui::InputText("エフェクト名", effectBuffer, sizeof(effectBuffer));
			entry.effectName = effectBuffer;

			Engine::ImGuiManager::Select("追従先", entry.parentName, comboSystem->GetParentTransforms());

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

			static const char* ComboEffectTriggerTypeLabels[] = {
				"時間範囲",
				"時間経過",
				"着地したら",
			};
			Engine::ImGuiManager::Select("発生条件", ComboEffectTriggerTypeLabels, entry.triggerType);

			shouldSyncSequence |= ImGui::DragFloat("開始時間", &entry.startTime, 0.01f, 0.0f, 60.0f, "%.2f");
			if (entry.triggerType != ComboEffectTriggerType::kTimer) {
				shouldSyncSequence |= ImGui::DragFloat("終了時間", &entry.endTime, 0.01f, 0.0f, 60.0f, "%.2f");
			}
			if (entry.triggerType == ComboEffectTriggerType::kTimeWindow) {
				ImGui::DragFloat("発生頻度", &entry.interval, 0.01f, 0.001f, 60.0f, "%.3f");
			}
			ImGui::DragFloat3("発生オフセット", &entry.offset.x, 0.01f);
			if (entry.endTime < entry.startTime) {
				entry.endTime = entry.startTime;
			}
			if (entry.interval < 0.001f) {
				entry.interval = 0.001f;
			}

			if (ImGui::Button("削除")) {
				removeIndex = i;
			}

			ImGui::Separator();
			ImGui::PopID();
		}

		if (removeIndex >= 0 && removeIndex < static_cast<int>(effect.comboEffects.size())) {
			effect.comboEffects.erase(effect.comboEffects.begin() + removeIndex);
			shouldSyncSequence = true;
		}

		if (ImGui::Button("コンボエフェクト追加")) {
			effect.comboEffects.push_back(ComboEffectEntry{
				.effectName = "",
				.parentName = "Player",
				.triggerType = ComboEffectTriggerType::kTimeWindow,
				.startTime = 0.0f,
				.endTime = 0.0f,
				.interval = 0.1f,
				.offset = { 0.0f, 0.0f, 0.0f }
				});
			shouldSyncSequence = true;
		}

		if (shouldSyncSequence) {
			SyncComboEffectSequenceEvents();
		}
	}

	void EditorBlock::SyncComboEffectSequenceEvents() {
		sequence_.RemoveEventsByPrefix("コンボエフェクト");
		for (int i = 0; i < static_cast<int>(data_.effect.comboEffects.size()); ++i) {
			const ComboEffectEntry& entry = data_.effect.comboEffects[i];
			sequence_.UpsertEvent(AttackEvent{
				.startFrame = ConvertUtility::SecondsToFrames(entry.startTime, 60.0f),
				.endFrame = ConvertUtility::SecondsToFrames(entry.endTime, 60.0f),
				.color = 0x00AAFFFF,
				.name = MakeComboEffectSequenceName(i)
				});
		}
	}

#pragma endregion // ImGui管理

#pragma endregion // コンボ単位管理
}
