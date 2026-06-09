#include "ComboButton.h"
#include "DirectXGame/application/base/Character/Base/CharacterContext.h"
#include "DirectXGame//application/base/Input/InputSystem.h"


// 押したら
bool Combo::ComboButton::IsPressed(const InputSystem& inputSystem) const {
	// コンボ用ボタンを入力システム用ボタンへ変換して、押下中か確認する
	return inputSystem.GetButtom(InputButton::kPressed, ConvertGamePadButton(button_));
}

// 押した瞬間
bool Combo::ComboButton::IsTriggered(const InputSystem& inputSystem) const {
	// このフレームに押された入力だけを拾う
	return inputSystem.GetButtom(InputButton::kTriggered, ConvertGamePadButton(button_));
}

// 離した瞬間
bool Combo::ComboButton::IsReleased(const InputSystem& inputSystem) const {
	// このフレームに離された入力だけを拾う
	return inputSystem.GetButtom(InputButton::kReleased, ConvertGamePadButton(button_));
}


// 押して反応する条件
bool Combo::ComboButton::IsInput(const InputSystem& inputSystem) const {

	// 設定された入力条件に合わせて、押下中・押した瞬間・離した瞬間を組み合わせる
	switch (type_)
	{
	case ComboButtonInputType::kPressed: // 押したら
		return IsPressed(inputSystem);
		break;
	case ComboButtonInputType::kTriggered: // 押した瞬間
		return IsTriggered(inputSystem);
		break;
	case ComboButtonInputType::kReleased: // 離した瞬間
		return IsReleased(inputSystem);
		break;
	case ComboButtonInputType::kPressTriggerReleased: // 押す、押した瞬間、離した瞬間
		return IsPressed(inputSystem) || IsTriggered(inputSystem) || IsReleased(inputSystem);
		break;
	case ComboButtonInputType::kPressTriggered:
		return IsPressed(inputSystem) || IsTriggered(inputSystem); // 押す、押した瞬間
		break;
	case ComboButtonInputType::kPressReleased:
		return IsPressed(inputSystem) || IsReleased(inputSystem); // 押す、離した瞬間
		break;
	case ComboButtonInputType::kTriggerReleased:
		return IsTriggered(inputSystem) || IsReleased(inputSystem); // 押した瞬間、離した瞬間
		break;
	default:	// 指定されたtypeでないのなら
		return false;
		break;
	}
}

#pragma region ComboSequence

/// <summary>
/// コンボボタンを順番に登録
/// </summary>
void Combo::ComboSequence::RegisterCombo(const std::vector<ComboButton>& buttons) {
	// 新しいコンボ入力列を登録するため、古い入力列を破棄する
	comboButtons_.clear();
	for (auto& b : buttons) {
		// ボタン設定は値としてコピーして保持する
		comboButtons_.emplace_back(b);
	}
}

/// <summary>
/// コンボ成立チェック
/// </summary>
bool Combo::ComboSequence::Update(const Character::CharacterContext& ctx) {
	if (comboButtons_.empty()) return false;
	// 入力判定
	// 現状は順番入力ではなく、登録されたどれかのボタンが成立したら次コンボへ進める
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
	// コンボデータ用の列挙値を、InputSystemが理解する列挙値へ対応付ける
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
