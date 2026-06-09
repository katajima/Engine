#include "EnemyAiDebugSystem.h"
#include "EnemyAiSystem.h"
#include "EnemyAttackRequest.h"
#include "EnemyAttackSlotSystem.h"
#include "EnemyCrowdBehaviorData.h"
#include "EnemyCrowdData.h"
#include "EnemyCrowdSystem.h"
#include "DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "DirectXGame/engine/Line/LineCommon.h"

#ifdef _DEBUG
#include "imgui.h"
#endif

namespace Character {
#ifdef _DEBUG
	namespace {
		// 攻撃リングをImGui表示用の文字列に変換する
		const char* ToText(EnemyAttackRing ring) {
			switch (ring) {
			case EnemyAttackRing::TooClose: return "TooClose";
			case EnemyAttackRing::Attack: return "Attack";
			case EnemyAttackRing::Standby: return "Standby";
			case EnemyAttackRing::Approach: return "Approach";
			case EnemyAttackRing::None:
			default: return "None";
			}
		}

		// 群衆レイヤーをImGui表示用の文字列に変換する
		const char* ToText(EnemyCrowdLayer layer) {
			switch (layer) {
			case EnemyCrowdLayer::Attack: return "Attack";
			case EnemyCrowdLayer::Standby: return "Standby";
			case EnemyCrowdLayer::Approach:
			default: return "Approach";
			}
		}

		// 群衆の行動タイプをImGui表示用の文字列に変換する
		const char* ToText(EnemyCrowdBehaviorType type) {
			switch (type) {
			case EnemyCrowdBehaviorType::Rush: return "Rush";
			case EnemyCrowdBehaviorType::Encircle: return "Encircle";
			case EnemyCrowdBehaviorType::WaveAssault: return "Wave Assault";
			case EnemyCrowdBehaviorType::Formation: return "Formation";
			case EnemyCrowdBehaviorType::FlowCrowd: return "Flow Crowd";
			case EnemyCrowdBehaviorType::PressureCrowd: return "Pressure Crowd";
			case EnemyCrowdBehaviorType::DistributedEncircle: return "Distributed Encircle";
			case EnemyCrowdBehaviorType::DensityAdaptive: return "Density Adaptive";
			case EnemyCrowdBehaviorType::Spiral: return "Spiral";
			case EnemyCrowdBehaviorType::Hunting: return "Hunting";
			case EnemyCrowdBehaviorType::Flocking:
			default: return "Flocking";
			}
		}

		// 役割レイヤーごとにデバッグ線の色を変える
		Vector4 LayerColor(EnemyCrowdLayer layer) {
			switch (layer) {
			case EnemyCrowdLayer::Attack: return { 1.0f, 0.2f, 0.2f, 1.0f };
			case EnemyCrowdLayer::Standby: return { 1.0f, 0.8f, 0.1f, 1.0f };
			case EnemyCrowdLayer::Approach:
			default: return { 0.1f, 0.9f, 0.4f, 1.0f };
			}
		}
	}
#endif

	void EnemyAiDebugSystem::Initialize(Engine::LineCommon* lineCommon) {
		// デバッグ線を描画するための共通描画クラスを保持する
		lineCommon_ = lineCommon;
	}

	void EnemyAiDebugSystem::Update(const std::vector<BaseEnemy*>& enemies, const EnemyAiSystem* aiSystem) {
#ifdef _DEBUG
		// Debugビルドでは操作パネルとワールド上の補助線を更新する
		DrawControlPanel(enemies, aiSystem);
		if (isVisible_ && lineCommon_ && aiSystem) {
			DrawWorldDebug(enemies, aiSystem);
		}
#else
		// Releaseビルドでは引数未使用警告を抑える
		(void)enemies;
		(void)aiSystem;
#endif
	}

#ifdef _DEBUG
	void EnemyAiDebugSystem::DrawControlPanel(const std::vector<BaseEnemy*>& enemies, const EnemyAiSystem* aiSystem) {
		// ImGuiウィンドウが折りたたまれている時は中身を描画しない
		if (!ImGui::Begin("Enemy AI Debug")) {
			ImGui::End();
			return;
		}

		// 表示したいデバッグ情報だけを切り替えられるようにする
		ImGui::Checkbox("AI表示", &isVisible_);
		ImGui::Checkbox("最終移動目標", &showMoveTarget_);
		ImGui::Checkbox("隊形スロット", &showFormationSlot_);
		ImGui::Checkbox("フロー方向", &showFlowDirection_);
		ImGui::Checkbox("攻撃スロット", &showAttackSlots_);
		ImGui::Checkbox("敵ごとの判断", &showEnemyDetails_);
		ImGui::Separator();

		int ringCount[5]{};
		int layerCount[3]{};
		int grantedCount = 0;
		// 現在の敵全体の攻撃リングと群衆レイヤーの数を集計する
		for (BaseEnemy* enemy : enemies) {
			if (!enemy) {
				continue;
			}

			EnemyAttackRing ring = EnemyAttackRing::None;
			if (EnemyAttackRequest* request = enemy->GetEnemyAttackSystem()->GetAttackRequest()) {
				ring = request->GetRing();
				if (request->GetIsGranted()) {
					grantedCount++;
				}
			}
			ringCount[static_cast<int>(ring)]++;

			if (aiSystem) {
				if (const EnemyFlockingSteering* steering = aiSystem->GetCrowdSystem()->FindSteering(enemy)) {
					layerCount[static_cast<int>(steering->layer)]++;
				}
			}
		}

		ImGui::Text("対象敵数: %d / 攻撃許可: %d", static_cast<int>(enemies.size()), grantedCount);
		ImGui::Text("Ring  Attack:%d  Approach:%d  Standby:%d  TooClose:%d",
			ringCount[static_cast<int>(EnemyAttackRing::Attack)],
			ringCount[static_cast<int>(EnemyAttackRing::Approach)],
			ringCount[static_cast<int>(EnemyAttackRing::Standby)],
			ringCount[static_cast<int>(EnemyAttackRing::TooClose)]);
		ImGui::Text("Layer Attack:%d  Approach:%d  Standby:%d",
			layerCount[static_cast<int>(EnemyCrowdLayer::Attack)],
			layerCount[static_cast<int>(EnemyCrowdLayer::Approach)],
			layerCount[static_cast<int>(EnemyCrowdLayer::Standby)]);

		if (showEnemyDetails_ && aiSystem) {
			ImGui::Separator();
			// 敵ごとの役割、攻撃許可、群衆内番号を一覧表示する
			for (BaseEnemy* enemy : enemies) {
				if (!enemy) {
					continue;
				}

				EnemyAttackRequest* request = enemy->GetEnemyAttackSystem()->GetAttackRequest();
				const EnemyFlockingSteering* steering = aiSystem->GetCrowdSystem()->FindSteering(enemy);
				const EnemyAttackRing ring = request ? request->GetRing() : EnemyAttackRing::None;
				const EnemyCrowdLayer layer = steering ? steering->layer : EnemyCrowdLayer::Approach;
				const bool granted = request && request->GetIsGranted();

				ImGui::BulletText("#%u G%d  %s / %s / %s%s",
					enemy->GetCrowdMemberIndex(),
					enemy->GetCrowdGroupId(),
					ToText(enemy->GetCrowdBehavior().type),
					ToText(layer),
					ToText(ring),
					granted ? " [TOKEN]" : "");
				if (steering && (steering->isCommander || steering->isTargetHeld)) {
					ImGui::SameLine();
					ImGui::Text("%s%s",
						steering->isCommander ? " [LEADER]" : "",
						steering->isTargetHeld ? " [HOLD]" : "");
				}
			}
		}

		ImGui::End();
	}

	void EnemyAiDebugSystem::DrawWorldDebug(const std::vector<BaseEnemy*>& enemies, const EnemyAiSystem* aiSystem) {
		// LineCommonのデバッグメッシュへ、AI判断を示す補助線を追加する
		Engine::LineMeshData& lines = lineCommon_->GetDebugLineMeshData();
		EnemyCrowdSystem* crowdSystem = aiSystem->GetCrowdSystem();

		for (BaseEnemy* enemy : enemies) {
			if (!enemy) {
				continue;
			}

			const Vector3 start = enemy->GetWorldPosition() + Vector3{ 0.0f, 0.4f, 0.0f };
			const EnemyFlockingSteering* steering = crowdSystem->FindSteering(enemy);
			const Vector4 color = steering ? LayerColor(steering->layer) : Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };

			if (showMoveTarget_ && steering) {
				// 敵から最終移動目標までの線と目標球を描画する
				const Vector3 moveTarget = steering->moveTarget + Vector3{ 0.0f, 0.4f, 0.0f };
				lines.AddLine(start, moveTarget, color);
				lines.AddLineSphere(Sphere{ moveTarget, 0.32f }, color, 4, 4);
			}

			if (showFormationSlot_) {
				if (const CrowdSlot* slot = crowdSystem->FindSlot(enemy)) {
					// 群衆パターンで割り当てられた隊形スロットを描画する
					const Vector3 position = slot->position + Vector3{ 0.0f, 0.2f, 0.0f };
					lines.AddLine(start, position, { 0.2f, 0.7f, 1.0f, 1.0f });
					lines.AddLineSphere(Sphere{ position, 0.24f }, { 0.2f, 0.7f, 1.0f, 1.0f }, 4, 4);
				}
			}

			if (showFlowDirection_ && steering) {
				// フロー方向を短い線で表示し、群衆の流れを確認できるようにする
				const Vector3 directionEnd = start + steering->flowDirection * 2.0f;
				lines.AddLine(start, directionEnd, { 0.2f, 0.9f, 1.0f, 1.0f });
			}

			if (steering && steering->isCommander) {
				// 統率者は頭上の球で強調する
				lines.AddLineSphere(Sphere{ start + Vector3{ 0.0f, 0.8f, 0.0f }, 0.55f }, { 1.0f, 0.95f, 0.2f, 1.0f }, 6, 4);
			}
		}

		if (showAttackSlots_) {
			// 攻撃スロットは使用中かどうかで色を変えて描画する
			for (const AttackSlot& slot : aiSystem->GetAttackSlotSystem()->GetSlots()) {
				const Vector4 color = slot.occupied ?
					Vector4{ 1.0f, 0.25f, 0.15f, 1.0f } :
					Vector4{ 0.45f, 0.45f, 0.45f, 1.0f };
				const Vector3 position = slot.position + Vector3{ 0.0f, 0.15f, 0.0f };
				lines.AddLineSphere(Sphere{ position, 0.45f }, color, 6, 4);
			}
		}
	}
#endif
}
