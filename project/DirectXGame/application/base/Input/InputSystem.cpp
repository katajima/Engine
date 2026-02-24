#include "InputSystem.h"

void InputSystem::Initialize(Engine::Input* input) {
	this->input = input;
}

void InputSystem::Update(float dt) {

	// コントローラが接続されているか取得
	data_.isControllerConnected = input->IsControllerConnected();

	// 移動スティック取得
	data_.moveShick = input->GetGamePadLeftStick();

	// 視点スティック取得
	data_.lookStick = input->GetGamePadRightStick();

	// ジャンプ入力押している間
	data_.jumpPressed = input->IsGamePadPressed(GamePadButton::GAMEPAD_A);
	// ジャンプ入力押した瞬間
	data_.jumpTrigger = input->IsGamePadTriggered(GamePadButton::GAMEPAD_A);

	// 回避入力押した瞬間
	data_.dodgeTrigger = input->IsGamePadTriggered(GamePadButton::GAMEPAD_LB);

	// ダッシュ入力している間
	float lt = input->GetGamePadLeftTrigger();
	const float triggerDeadZone = 0.2f;
	data_.dashHeld = lt > triggerDeadZone;

	// ロックオン入力している間
	float rt = input->GetGamePadRightTrigger();
	data_.lockOnHeld = rt > triggerDeadZone;

	// スキル入力押した瞬間
	data_.skillTrigger = input->IsGamePadTriggered(GamePadButton::GAMEPAD_Y);

	// 必殺技入力押した瞬間
	data_.specialTrigger = input->IsGamePadTriggered(GamePadButton::GAMEPAD_RB);

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
