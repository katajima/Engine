#include "DummyEnemy.h"
#include"DirectXGame/application/base/Effect/Effect.h"
#include"DirectXGame/application/base/Character/Move/Base/MoveComponent.h"
#include "DirectXGame/engine/Entity/ObjectComponent.h"
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include "DirectXGame/engine/Move/RigidBodyComponent.h"
#include "imgui.h"

namespace Character {
	void DummyEnemy::Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) {
		// 基盤初期化
		BaseInitialize(inputSystem, entityManager, globalVariables, position, camera, "enemyBodySG01.obj", "DummyEnemy");
		// サイズ
		Vector3 size = { 0.9f,0.9f,0.9f };
		objectComponent_->SetInstancingSRT(size, {}, position);	// SRT設定

		// デバッグ用の帰還位置を初期配置で保持
		returnPosition_ = position;

		moveComponent_->GetMoveSystem()->Data().maxSpeed = 0.0f;

		// パラメーター初期化
		Parameters()->HP.Initiaize(hp, 0, hp, 0);
		Parameters()->strength = 10.0f;

		// エフェクト用のトランスフォーム初期化
		worldEffect_ = std::make_unique<Engine::WorldTransform>();
		worldEffect_->Initialize();
		worldEffect_->parent_ = &objectComponent_->GetWorldTransform();
		worldEffect_->translate_ = { 0,1,0 };

		// 保存項目初期化
		InitializeBaseAddItem();
		// トランスフォーム更新
		GetWorldTransform().Update();
	}

	void DummyEnemy::Update() {
		// 基盤の更新
		BaseUpdate();

		// 被弾した瞬間にデバッグ挙動を反映
		ApplyDebugBehaviorOnHit();

		// 地上/空中固定を有効にしている場合は検証姿勢を維持
		ApplyDummyTypeLock();

		// 完全停止中はリアクション移動後も指定位置へ戻す
		if (completeStop_) {
			ResetToDebugReturnPosition();
		}

		// 状態をモデルカラーで見分けられるようにする
		ApplyDebugStateColor();

		// ダミー検証中に倒れないようHPを常に最大へ戻す
		Parameters()->HP.value = 100.0f;

		// ダミーの状態確認と挙動切り替えUI
		UpdateDebugImGui();
	}

	void DummyEnemy::UpdateDebugImGui() {
		// 表示自体を切れるようにして、検証画面の邪魔にならないようにする
		if (!showDebugWindow_) {
			return;
		}

		const CharacterMainState mainState = GetCurrentMainState();				// 現在のメイン状態
		HitMotionSystem* hitMotion = GetHitMotionSystem();						// ヒットリアクション情報
		Engine::RigidBodyComponent* rigid = objectComponent_->GetRigidBodyComponent(); // 物理速度確認用
		Vector3 worldPos = GetWorldTransform().GetWorldPosition();				// 現在位置
		Vector3 velocity = rigid ? rigid->GetVelocity() : Vector3{};			// 現在速度
		int dummyTypeIndex = static_cast<int>(dummyType_);						// ImGui用の選択番号
		const char* dummyTypeLabels[] = { "Ground", "Air" };					// ダミータイプ表示名

		ImGui::Begin("DummyEnemy Debug", &showDebugWindow_);

		ImGui::SeparatorText("Basic");
		ImGui::Text("Name        : %s", GetName().c_str());
		ImGui::Text("Main State  : %s", GetMainStateName(mainState));
		ImGui::Text("HP          : %.1f / %.1f", Parameters()->HP.value, Parameters()->HP.maxValue);
		ImGui::Text("Position    : %.2f, %.2f, %.2f", worldPos.x, worldPos.y, worldPos.z);
		ImGui::Text("Velocity    : %.2f, %.2f, %.2f", velocity.x, velocity.y, velocity.z);

		ImGui::SeparatorText("Hit Reaction");
		ImGui::Text("Hit State   : %s", hitMotion ? GetHitMotionStateName(hitMotion->GetHitMotionState()) : "None");
		ImGui::Text("Hit Motion  : %s", hitMotion && hitMotion->IsHitMotion() ? "true" : "false");
		ImGui::Text("Hit Stun    : %s / %.2f", hitMotion && hitMotion->IsHitStun() ? "true" : "false",
			hitMotion ? hitMotion->GetHitStunTimer() : 0.0f);
		ImGui::Text("Down        : %s / %.2f", hitMotion && hitMotion->IsDown() ? "true" : "false",
			hitMotion ? hitMotion->GetDownTimer() : 0.0f);
		ImGui::Text("Launch      : %s", hitMotion && hitMotion->IsLaunch() ? "true" : "false");
		ImGui::Text("Hit Stop    : %s", hitMotion && hitMotion->IsHitStop() ? "true" : "false");
		ImGui::Checkbox("Use State Color", &useDebugStateColor_);
		ImGui::Text("Color Memo  : White=Normal, Yellow=HitStop, Orange=Stun, Cyan=Launch, Blue=Down, Red=Damage");

		ImGui::SeparatorText("Behavior");
		ImGui::Checkbox("Complete Stop", &completeStop_);
		ImGui::Checkbox("Return On Hit", &returnOnHit_);
		ImGui::Checkbox("Recover Immediately On Hit", &recoverImmediatelyOnHit_);
		ImGui::DragFloat3("Return Position", &returnPosition_.x, 0.1f);
		if (ImGui::Button("Set Return Position From Current")) {
			returnPosition_ = worldPos;
		}
		ImGui::SameLine();
		if (ImGui::Button("Return Now")) {
			ResetToDebugReturnPosition();
		}

		ImGui::SeparatorText("Dummy Type");
		if (ImGui::Combo("Type", &dummyTypeIndex, dummyTypeLabels, 2)) {
			dummyType_ = static_cast<DummyType>(dummyTypeIndex);
		}
		ImGui::Checkbox("Lock Ground/Air State", &lockDummyType_);
		ImGui::DragFloat("Ground Height", &groundHeight_, 0.1f);
		ImGui::DragFloat("Air Height", &airHeight_, 0.1f);

		ImGui::End();
	}

	void DummyEnemy::ApplyDebugBehaviorOnHit() {
		const bool isDamageState = GetCurrentMainState() == CharacterMainState::Damage; // 現在Damageか
		const bool enteredDamageState = isDamageState && !wasDamageState_;				// 今フレームでDamageへ入ったか

		if (enteredDamageState) {
			if (returnOnHit_) {
				ResetToDebugReturnPosition();
			}
			if (recoverImmediatelyOnHit_) {
				GetHitMotionSystem()->ForceFinishReaction();
				GetCharacterStateMachine()->ChangeState(CharacterMainState::Move);
			}
		}

		wasDamageState_ = isDamageState;
	}

	void DummyEnemy::ApplyDummyTypeLock() {
		// 固定が無効なら、タイプ表示だけを使って挙動には干渉しない
		if (!lockDummyType_) {
			return;
		}

		Engine::RigidBodyComponent* rigid = objectComponent_->GetRigidBodyComponent(); // 物理速度リセット用
		Engine::WorldTransform& world = GetWorldTransform();							// 位置固定対象

		// ダミータイプに応じてY座標を固定し、リアクション検証時の地上/空中条件を安定させる
		world.translate_.y = dummyType_ == DummyType::Ground ? groundHeight_ : airHeight_;
		world.Update();

		if (rigid) {
			rigid->ResetAcceleration();
			rigid->ResetVelocity();
			rigid->SetIsGravity(false);
		}
	}

	void DummyEnemy::ApplyDebugStateColor() {
		// 色表示を使わない場合は通常色へ戻す
		if (!useDebugStateColor_) {
			objectComponent_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
			return;
		}

		HitMotionSystem* hitMotion = GetHitMotionSystem(); // 色分けに使うヒットリアクション情報

		// 優先度の高い一瞬の状態から順に判定する
		if (hitMotion && hitMotion->IsHitStop()) {
			objectComponent_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f }); // ヒットストップ中は黄色
			return;
		}
		if (hitMotion && hitMotion->IsDown()) {
			objectComponent_->SetColor({ 0.1f, 0.25f, 1.0f, 1.0f }); // ダウン中は青
			return;
		}
		if (hitMotion && hitMotion->IsLaunch()) {
			objectComponent_->SetColor({ 0.0f, 0.9f, 1.0f, 1.0f }); // 打ち上げ中はシアン
			return;
		}
		if (hitMotion && hitMotion->IsHitStun()) {
			objectComponent_->SetColor({ 1.0f, 0.45f, 0.0f, 1.0f }); // スタン中はオレンジ
			return;
		}
		if (GetCurrentMainState() == CharacterMainState::Damage) {
			objectComponent_->SetColor({ 1.0f, 0.1f, 0.1f, 1.0f }); // 被弾状態は赤
			return;
		}

		// 通常時は白に戻す
		objectComponent_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	}

	void DummyEnemy::ResetToDebugReturnPosition() {
		Engine::WorldTransform& world = GetWorldTransform();	// 座標を戻す対象
		Engine::RigidBodyComponent* rigid = objectComponent_->GetRigidBodyComponent(); // 物理速度リセット用

		world.translate_ = returnPosition_;
		world.Update();

		if (rigid) {
			rigid->ResetAcceleration();
			rigid->ResetVelocity();
		}
	}

	const char* DummyEnemy::GetMainStateName(CharacterMainState state) const {
		// ImGui表示用にメインステートを短い英字へ変換
		switch (state) {
		case CharacterMainState::Idle: return "Idle";
		case CharacterMainState::Move: return "Move";
		case CharacterMainState::Jump: return "Jump";
		case CharacterMainState::Avoidance: return "Avoidance";
		case CharacterMainState::Defense: return "Defense";
		case CharacterMainState::Attack: return "Attack";
		case CharacterMainState::Die: return "Die";
		case CharacterMainState::Damage: return "Damage";
		case CharacterMainState::Dash: return "Dash";
		case CharacterMainState::Fainting: return "Fainting";
		case CharacterMainState::Skill: return "Skill";
		case CharacterMainState::Special: return "Special";
		default: return "Unknown";
		}
	}

	const char* DummyEnemy::GetHitMotionStateName(HitMotionSystem::HitMotionState state) const {
		// ImGui表示用にヒットリアクション状態を短い英字へ変換
		switch (state) {
		case HitMotionSystem::HitMotionState::None: return "None";
		case HitMotionSystem::HitMotionState::HitStop: return "HitStop";
		case HitMotionSystem::HitMotionState::Knockback: return "Knockback";
		case HitMotionSystem::HitMotionState::BlowAway: return "BlowAway";
		case HitMotionSystem::HitMotionState::Launch: return "Launch";
		case HitMotionSystem::HitMotionState::WallBounce: return "WallBounce";
		case HitMotionSystem::HitMotionState::Down: return "Down";
		default: return "Unknown";
		}
	}


	void DummyEnemy::InitStateMachine() {
		// ステートマシーン初期化
		stateMachine_ = std::make_unique<CharacterStateMachine>();
		stateMachine_->RegisterState(CharacterMainState::Move, [](BaseCharacter* p) {
			return std::make_unique<DummyEnemyMoveState>(p);
			});
		stateMachine_->RegisterState(CharacterMainState::Die, [](BaseCharacter* p) {
			return std::make_unique<DummyEnemyDieState>(p);
			});
		stateMachine_->RegisterState(CharacterMainState::Damage, [](BaseCharacter* p) {
			return std::make_unique<DummyEnemyDamageState>(p);
			});
		stateMachine_->Init(this, CharacterMainState::Move);
	}
}
