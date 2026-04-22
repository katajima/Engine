#include "ComboButton.h"

#pragma region ComboSequence

/// <summary>
/// コンボボタンを順番に登録
/// </summary>
void Combo::ComboSequence::RegisterCombo(const std::vector<ComboButton>& buttons) {
	comboButtons_.clear();
	for (auto& b : buttons) {
		comboButtons_.emplace_back(b);
	}
}

/// <summary>
/// コンボ成立チェック
/// </summary>
bool Combo::ComboSequence::Update(const Character::CharacterContext& ctx) {
	if (comboButtons_.empty()) return false;
	// 入力判定
	for (auto& bu : comboButtons_) {
		if (bu.IsInput(*ctx.input)) {
			return true;
		}
	}
	return false;
}

#pragma endregion // コンボボタン

GamePadButton Combo::ConvertGamePadButton(ComboGamePadButton button)
{
	switch (button)
	{
	case Combo::ComboGamePadButton::GAMEPAD_Up:
		return GamePadButton::GAMEPAD_Up;
		break;
	case Combo::ComboGamePadButton::GAMEPAD_Down:
		return GamePadButton::GAMEPAD_Down;
		break;
	case Combo::ComboGamePadButton::GAMEPAD_Left:
		return GamePadButton::GAMEPAD_Left;
		break;
	case Combo::ComboGamePadButton::GAMEPAD_Right:
		return GamePadButton::GAMEPAD_Right;
		break;
	case Combo::ComboGamePadButton::GAMEPAD_A:
		return GamePadButton::GAMEPAD_A;
		break;
	case Combo::ComboGamePadButton::GAMEPAD_B:
		return GamePadButton::GAMEPAD_B;
		break;
	case Combo::ComboGamePadButton::GAMEPAD_X:
		return GamePadButton::GAMEPAD_X;
		break;
	case Combo::ComboGamePadButton::GAMEPAD_Y:
		return GamePadButton::GAMEPAD_Y;
		break;
	case Combo::ComboGamePadButton::GAMEPAD_LB:
		return GamePadButton::GAMEPAD_LB;
		break;
	case Combo::ComboGamePadButton::GAMEPAD_RB:
		return GamePadButton::GAMEPAD_RB;
		break;
	case Combo::ComboGamePadButton::GAMEPAD_LS:
		return GamePadButton::GAMEPAD_LS;
		break;
	case Combo::ComboGamePadButton::GAMEPAD_RS:
		return GamePadButton::GAMEPAD_RS;
		break;
	case Combo::ComboGamePadButton::GAMEPAD_Start:
		return GamePadButton::GAMEPAD_Start;
		break;
	case Combo::ComboGamePadButton::GAMEPAD_Back:
		return GamePadButton::GAMEPAD_Back;
		break;
	case Combo::ComboGamePadButton::GAMEPAD_Max:
		return GamePadButton::GAMEPAD_Max;
		break;
	default:
		return GamePadButton::GAMEPAD_Max;
		break;
	}
}
