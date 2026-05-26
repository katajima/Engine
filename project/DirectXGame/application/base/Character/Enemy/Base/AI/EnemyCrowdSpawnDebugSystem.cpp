#include "EnemyCrowdSpawnDebugSystem.h"

#include "DirectXGame/application/base/Character/Base/CharacterManeger.h"
#include "DirectXGame/application/base/Character/Enemy/Base/EnemyData.h"
#include "DirectXGame/engine/struct/Structs3D.h"

#ifdef _DEBUG
#include <map>

#include "imgui.h"
#endif

namespace Character {
#ifdef _DEBUG
	namespace {
		const char* ToText(EnemyCrowdBehaviorType type) {
			switch (type) {
			case EnemyCrowdBehaviorType::Rush: return "直進型";
			case EnemyCrowdBehaviorType::Encircle: return "包囲型";
			case EnemyCrowdBehaviorType::WaveAssault: return "波状型";
			case EnemyCrowdBehaviorType::Flocking: return "群れ型";
			case EnemyCrowdBehaviorType::Formation: return "隊列型";
			case EnemyCrowdBehaviorType::FlowCrowd: return "流動型";
			case EnemyCrowdBehaviorType::PressureCrowd: return "圧迫型";
			case EnemyCrowdBehaviorType::DistributedEncircle: return "分散包囲型";
			case EnemyCrowdBehaviorType::DensityAdaptive: return "密度制御型";
			case EnemyCrowdBehaviorType::Spiral: return "スパイラル型";
			case EnemyCrowdBehaviorType::Hunting: return "ハンティング型";
			default: return "不明";
			}
		}

		struct CrowdSummary {
			int count = 0;
			int smallMeleeCount = 0;
			int smallRangedCount = 0;
			int mediumMeleeCount = 0;
			int dummyCount = 0;
			EnemyCrowdBehaviorType behaviorType = EnemyCrowdBehaviorType::Flocking;
		};
	}
#endif

	void EnemyCrowdSpawnDebugSystem::Update(CharacterManager* characterManager) {
#ifdef _DEBUG
		if (!ImGui::Begin("Enemy Crowd Spawn")) {
			ImGui::End();
			return;
		}

		ImGui::Text("出現位置と範囲");
		ImGui::DragFloat3("出現位置", &spawnPosition_.x, 0.25f);
		ImGui::DragFloat("出現範囲 X", &spawnAreaSize_.x, 0.25f, 0.0f, 100.0f);
		ImGui::DragFloat("出現範囲 Z", &spawnAreaSize_.z, 0.25f, 0.0f, 100.0f);

		ImGui::Separator();
		ImGui::Text("群衆行動");
		static const char* behaviorNames[] = {
			"直進型 (Rush)",
			"包囲型 (Encircle)",
			"波状型 (Wave Assault)",
			"群れ型 (Flocking)",
			"隊列型 (Formation)",
			"流動型 (Flow Crowd)",
			"圧迫型 (Pressure Crowd)",
			"分散包囲型 (Distributed Encircle)",
			"密度制御型 (Density Adaptive)",
			"スパイラル型 (Spiral)",
			"ハンティング型 (Hunting)",
		};
		int behaviorIndex = static_cast<int>(behaviorType_);
		if (ImGui::Combo("群衆タイプ", &behaviorIndex, behaviorNames, IM_ARRAYSIZE(behaviorNames))) {
			behaviorType_ = static_cast<EnemyCrowdBehaviorType>(behaviorIndex);
		}

		// パターン変更を試しやすいよう、利用可能な設定値を常に編集できるようにする
		ImGui::DragFloat("基準半径", &radius_, 0.1f, 1.0f, 50.0f);
		ImGui::DragFloat("隊列間隔", &spacing_, 0.1f, 0.1f, 20.0f);
		ImGui::DragFloat("回転速度", &angularSpeed_, 0.05f, -5.0f, 5.0f);
		ImGui::DragFloat("波の切替秒数", &waveInterval_, 0.1f, 0.1f, 20.0f);
		ImGui::DragInt("波の列数", &waveCount_, 1.0f, 1, 10);

		static const char* formationNames[] = { "横列", "円形", "V字", "方陣" };
		int formationIndex = static_cast<int>(formationShape_);
		if (ImGui::Combo("隊列の陣形", &formationIndex, formationNames, IM_ARRAYSIZE(formationNames))) {
			formationShape_ = static_cast<EnemyFormationShape>(formationIndex);
		}

		ImGui::Separator();
		ImGui::Text("敵の種類と人数");
		ImGui::DragInt("小型近接", &smallMeleeCount_, 1.0f, 0, 60);
		ImGui::DragInt("小型遠距離", &smallRangedCount_, 1.0f, 0, 60);
		ImGui::DragInt("中型近接", &mediumMeleeCount_, 1.0f, 0, 60);
		ImGui::DragInt("ダミー", &dummyCount_, 1.0f, 0, 60);

		const int totalCount = smallMeleeCount_ + smallRangedCount_ + mediumMeleeCount_ + dummyCount_;
		ImGui::Text("生成合計: %d", totalCount);

		if (ImGui::Button("群衆を出現") && totalCount > 0) {
			SpawnConfiguredCrowd(characterManager);
		}
		ImGui::SameLine();
		if (ImGui::Button("人数をクリア")) {
			smallMeleeCount_ = 0;
			smallRangedCount_ = 0;
			mediumMeleeCount_ = 0;
			dummyCount_ = 0;
		}

		ImGui::Separator();
		DrawActiveCrowds(characterManager);

		ImGui::End();
#else
		(void)characterManager;
#endif
	}

#ifdef _DEBUG
	CrowdBehaviorSettings EnemyCrowdSpawnDebugSystem::BuildBehaviorSettings() const {
		CrowdBehaviorSettings settings{};
		settings.type = behaviorType_;
		settings.formation = formationShape_;
		settings.radius = radius_;
		settings.spacing = spacing_;
		settings.angularSpeed = angularSpeed_;
		settings.waveInterval = waveInterval_;
		settings.waveCount = static_cast<uint32_t>(waveCount_);
		return settings;
	}

	void EnemyCrowdSpawnDebugSystem::SpawnConfiguredCrowd(CharacterManager* characterManager) {
		if (!characterManager) {
			return;
		}

		const Vector3 halfArea = { spawnAreaSize_.x * 0.5f, 0.0f, spawnAreaSize_.z * 0.5f };
		const AABB spawnArea = {
			spawnPosition_ - halfArea,
			spawnPosition_ + halfArea
		};
		const CrowdBehaviorSettings settings = BuildBehaviorSettings();
		const int groupId = nextGroupId_++;

		// 種類が異なる敵も同じgroupIdを持つため、ひとつの群衆として協調して動く
		characterManager->CreateEnemyGroup(EnemyType::kSmallMelee, groupId, smallMeleeCount_, spawnPosition_, spawnArea, settings);
		characterManager->CreateEnemyGroup(EnemyType::kSmallRanged, groupId, smallRangedCount_, spawnPosition_, spawnArea, settings);
		characterManager->CreateEnemyGroup(EnemyType::kMediumMelee, groupId, mediumMeleeCount_, spawnPosition_, spawnArea, settings);
		characterManager->CreateEnemyGroup(EnemyType::kDummy, groupId, dummyCount_, spawnPosition_, spawnArea, settings);
	}

	void EnemyCrowdSpawnDebugSystem::DrawActiveCrowds(CharacterManager* characterManager) {
		if (!characterManager || !ImGui::CollapsingHeader("現在の群衆", ImGuiTreeNodeFlags_DefaultOpen)) {
			return;
		}

		std::map<int, CrowdSummary> activeCrowds;
		for (BaseCharacter* character : characterManager->GetCharacters()) {
			if (!character || character->GetCharacterType() != Type::Enemy) {
				continue;
			}

			BaseEnemy* enemy = static_cast<BaseEnemy*>(character);
			if (!enemy->GetAlive() || enemy->IsWaveExiting()) {
				continue;
			}

			CrowdSummary& summary = activeCrowds[enemy->GetCrowdGroupId()];
			summary.count++;
			summary.behaviorType = enemy->GetCrowdBehavior().type;
			switch (enemy->GetType()) {
			case EnemyType::kSmallMelee:
				summary.smallMeleeCount++;
				break;
			case EnemyType::kSmallRanged:
				summary.smallRangedCount++;
				break;
			case EnemyType::kMediumMelee:
				summary.mediumMeleeCount++;
				break;
			case EnemyType::kDummy:
				summary.dummyCount++;
				break;
			}
		}

		if (activeCrowds.empty()) {
			ImGui::Text("活動中の群衆はありません");
			return;
		}

		for (const auto& [groupId, summary] : activeCrowds) {
			ImGui::PushID(groupId);
			ImGui::Text("ID %d / %s / %d体", groupId, ToText(summary.behaviorType), summary.count);
			ImGui::Text("  近接:%d  遠距離:%d  中型:%d  ダミー:%d",
				summary.smallMeleeCount,
				summary.smallRangedCount,
				summary.mediumMeleeCount,
				summary.dummyCount);
			if (ImGui::Button("削除 (退場演出)")) {
				// 即座に消さず、既存のウェーブ終了と同じ自然な退場処理へ移行する
				characterManager->BeginEnemyCrowdExit(groupId);
			}
			ImGui::Separator();
			ImGui::PopID();
		}
	}
#endif
}
