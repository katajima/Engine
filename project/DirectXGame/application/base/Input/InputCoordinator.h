#pragma once

#include <memory>

#include "DirectXGame/application/base/Input/InputHander.h"
#include "DirectXGame/engine/input/InputManager.h"
#include "DirectXGame/application/base/Input/InputSystem.h"

namespace Engine {
	class Input;
}

namespace Character {
	class BaseCharacter;
}

/// <summary>
/// 物理入力、アクション入力、キャラクターコマンドを一括管理するクラス
/// </summary>
class InputCoordinator {
public:
	/// <summary>
	/// 3系統の入力管理を初期化し、標準キャラクターコマンドを登録する
	/// </summary>
	void Initialize(Engine::Input* input) {
		inputSystem_ = std::make_unique<InputSystem>();
		inputSystem_->Initialize(input);

		inputManager_ = std::make_unique<InputManager>();
		inputManager_->Initialize(input);

		inputHandler_ = std::make_unique<Character::InputHander>();
		inputHandler_->Initialize(input);
		RegisterDefaultCharacterCommands();
	}

	/// <summary>
	/// 入力状態だけを更新する
	/// </summary>
	void Update(float deltaTime) {
		inputSystem_->Update(deltaTime);
		inputManager_->Update(deltaTime);
		if (isPlayerInputBlocked_) {
			/// <summary>
			/// エディター操作中は移動やカメラなどが参照するプレイヤー入力だけを止める。
			/// </summary>
			inputSystem_->ClearKeyboardMousePlayerInput();
		}
	}

	/// <summary>
	/// 入力状態を更新し、成立した標準コマンドを対象キャラクターへ実行する
	/// </summary>
	void Update(float deltaTime, Character::BaseCharacter* owner) {
		SetOwner(owner);
		Update(deltaTime);

		Character::ICommand* command = inputHandler_->HandleInput();
		if (!isPlayerInputBlocked_ && command && owner) {
			command->Exec(*owner);
		}
	}

	/// <summary>
	/// キャラクターコマンドの実行対象を設定する
	/// </summary>
	void SetOwner(Character::BaseCharacter* owner) {
		inputManager_->SetOwner(owner);
	}

	/// <summary>
	/// カメラ、UI、ゲームシステムへ渡す従来形式の入力状態を取得する
	/// </summary>
	InputSystem* GetInputSystem() { return inputSystem_.get(); }
	const InputSystem* GetInputSystem() const { return inputSystem_.get(); }

	/// <summary>
	/// アクション入力管理を取得する
	/// </summary>
	InputManager* GetInputManager() { return inputManager_.get(); }

	/// <summary>
	/// プレイヤー操作入力の実行を止めるか設定する
	/// </summary>
	void SetPlayerInputBlocked(bool isBlocked) { isPlayerInputBlocked_ = isBlocked; }

private:
	/// <summary>
	/// 全プレイヤーシーンで共通のアクションとコマンドを登録する
	/// </summary>
	void RegisterDefaultCharacterCommands() {
		inputHandler_->Bind(
			[this] { return inputManager_->Triggered(InputManager::Action::Jump); },
			std::make_unique<Character::JampCommand>());
		inputHandler_->Bind(
			[this] { return inputManager_->Triggered(InputManager::Action::LightAttack); },
			std::make_unique<Character::AttackCommand>());
		inputHandler_->Bind(
			[this] { return inputManager_->Triggered(InputManager::Action::HeavyAttack); },
			std::make_unique<Character::HeavyAttackCommand>());
		inputHandler_->Bind(
			[this] { return inputManager_->Triggered(InputManager::Action::Skill); },
			std::make_unique<Character::SkillAttackCommand>());
		inputHandler_->Bind(
			[this] { return inputManager_->Triggered(InputManager::Action::Move); },
			std::make_unique<Character::MoveCommand>());
	}

private:
	std::unique_ptr<InputSystem> inputSystem_ = nullptr; // ゲーム全体が参照する入力状態
	std::unique_ptr<InputManager> inputManager_ = nullptr; // アクションへ変換した入力状態
	std::unique_ptr<Character::InputHander> inputHandler_ = nullptr; // 入力からコマンドを選択するハンドラー
	bool isPlayerInputBlocked_ = false; // エディター操作中などにプレイヤーだけ動かさない
};
