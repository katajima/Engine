#include "InputSystem.h"

void InputSystem::Initialize(Engine::Input* input) {
	this->input = input;
}

void InputSystem::Update(float dt) {
	// プレイヤー操作の入力データの更新
	PlayerInputUpdate(dt);
	// ゲーム操作の入力データの更新
	GameInputUpdate(dt);
}

void InputSystem::PlayerInputUpdate(float dt) {
	// コントローラが接続されているか取得
	playerInputData_.isControllerConnected = input->IsControllerConnected();

	// 移動スティック取得
	playerInputData_.moveShick = input->GetGamePadLeftStick();

	// 視点スティック取得
	playerInputData_.lookStick = input->GetGamePadRightStick();

	// ジャンプ入力押している間
	playerInputData_.jumpPressed = input->IsGamePadPressed(GamePadButton::GAMEPAD_A);
	// ジャンプ入力押した瞬間
	playerInputData_.jumpTrigger = input->IsGamePadTriggered(GamePadButton::GAMEPAD_A);

	// 回避入力押した瞬間
	playerInputData_.dodgeTrigger = input->IsGamePadTriggered(GamePadButton::GAMEPAD_LB);

	// ダッシュ入力している間
	float lt = input->GetGamePadLeftTrigger();
	const float triggerDeadZone = 0.2f;
	playerInputData_.dashHeld = lt > triggerDeadZone;

	// ロックオン入力している間
	float rt = input->GetGamePadRightTrigger();
	playerInputData_.lockOnHeld = rt > triggerDeadZone;

	// スキル入力押した瞬間
	playerInputData_.skillTrigger = input->IsGamePadTriggered(GamePadButton::GAMEPAD_B);

	// 必殺技入力押した瞬間
	playerInputData_.specialTrigger = input->IsGamePadTriggered(GamePadButton::GAMEPAD_RB);

}

void InputSystem::GameInputUpdate(float dt) {
	// 移動スティック取得
	gameInputData_.moveShick = input->GetGamePadLeftStick();
	// 決定入力押した瞬間
	gameInputData_.decisionTrigger = input->IsGamePadTriggered(GamePadButton::GAMEPAD_B);
	// ポーズ入力押した瞬間
	gameInputData_.pauseTrigger = input->IsGamePadTriggered(GamePadButton::GAMEPAD_Back);
}

bool InputSystem::GetButtom(InputButton press, GamePadButton button) const
{
	switch (press)
	{
	case InputButton::kPressed:
		return input->IsGamePadPressed(button);
		break;
	case InputButton::kTriggered:
		return input->IsGamePadTriggered(button);
		break;
	case InputButton::kReleased:
		return input->IsGamePadReleased(button);
		break;
	default:
		break;
	}

	return false;
}
