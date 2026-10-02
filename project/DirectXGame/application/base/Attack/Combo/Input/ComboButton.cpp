#include "ComboButton.h"
#include "DirectXGame/application/base/Character/Base/CharacterContext.h"
#include "DirectXGame//application/base/Input/InputSystem.h"
#include <algorithm>
#include <cmath>

namespace {
	// 方向入力が条件に一致するかを判定する共通処理です。
	bool MatchesComboDirection(Combo::ComboInputDirection direction, const Vector2& stick) {
		const float length = stick.Length();
		if (direction == Combo::ComboInputDirection::Any) return true;
		if (direction == Combo::ComboInputDirection::Neutral) return length < 0.25f;
		if (length < 0.25f) return false;
		if (direction == Combo::ComboInputDirection::Up) return stick.y >= std::abs(stick.x);
		if (direction == Combo::ComboInputDirection::Down) return -stick.y >= std::abs(stick.x);
		if (direction == Combo::ComboInputDirection::Right) return stick.x >= std::abs(stick.y);
		return -stick.x >= std::abs(stick.y);
	}
}


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

Combo::ComboButton::ComboButton(const ComboInputStep& step)
	: button_(step.button), type_(step.inputType) {
	// 設定データを実行用の入力条件へ展開する。
	SetStep(step);
}

void Combo::ComboButton::SetStep(const ComboInputStep& step) {
	// 入力ソースと長押し・溜め条件を1ステップへ反映する。
	button_ = step.button;
	type_ = step.inputType;
	source_ = step.source;
	action_ = step.action;
	direction_ = step.direction;
	minHoldSeconds_ = (std::max)(0.0f, step.minHoldSeconds);
	maxHoldSeconds_ = (std::max)(0.0f, step.maxHoldSeconds);
	minChargeRatio_ = (std::clamp)(step.minChargeRatio, 0.0f, 1.0f);
	maxChargeRatio_ = (std::clamp)(step.maxChargeRatio, minChargeRatio_, 1.0f);
	chargeSeconds_ = (std::max)(0.001f, step.chargeSeconds);
	ResetState();
}

void Combo::ComboButton::ResetState() const {
	// シーケンス再開時に前回の押下時間を持ち越さない。
	holdSeconds_ = 0.0f;
	releasedHoldSeconds_ = 0.0f;
	wasPressed_ = false;
}

bool Combo::ComboButton::IsInput(const Character::CharacterContext& ctx) const {
	// 入力なしキャラクターでは条件不成立として安全に扱う。
	if (source_ == ComboInputSource::GamePadButton && !ctx.input) return false;

	bool pressed = false;
	bool triggered = false;
	bool released = false;
	if (source_ == ComboInputSource::GamePadButton) {
		// ゲームパッド由来の押下状態を取得する。
		pressed = ctx.input->GetButtom(InputButton::kPressed, ConvertGamePadButton(button_));
		triggered = ctx.input->GetButtom(InputButton::kTriggered, ConvertGamePadButton(button_));
		released = ctx.input->GetButtom(InputButton::kReleased, ConvertGamePadButton(button_));
	}
	else {
		// キャラクター共通入力から攻撃・移動系アクションを取得する。
		switch (action_) {
		case ComboActionType::LightAttack:
			pressed = ctx.inputData.lightAttackPressed;
			triggered = ctx.inputData.lightAttackTrigger;
			released = ctx.inputData.lightAttackReleased;
			break;
		case ComboActionType::HeavyAttack:
			pressed = ctx.inputData.heavyAttackPressed;
			triggered = ctx.inputData.heavyAttackTrigger;
			released = ctx.inputData.heavyAttackReleased;
			break;
		case ComboActionType::Skill:
			pressed = ctx.inputData.skillPressed;
			triggered = ctx.inputData.skillTrigger;
			released = ctx.inputData.skillReleased;
			break;
		case ComboActionType::Jump:
			pressed = ctx.inputData.jumpPressed;
			triggered = ctx.inputData.jumpTrigger;
			break;
		case ComboActionType::Dodge:
			triggered = ctx.inputData.dodgeTrigger;
			break;
		case ComboActionType::Guard:
			pressed = ctx.inputData.guardPressed;
			triggered = ctx.inputData.guardTrigger;
			released = ctx.inputData.guardReleased;
			break;
		case ComboActionType::Special:
			triggered = ctx.inputData.specialTrigger;
			break;
		}
	}

	// 押下時間と離した直前の保持時間を更新する。
	if (pressed) {
		if (!wasPressed_) holdSeconds_ = 0.0f;
		holdSeconds_ += (std::max)(ctx.dt, 0.0f);
	}
	else if (wasPressed_) {
		releasedHoldSeconds_ = holdSeconds_;
		holdSeconds_ = 0.0f;
	}
	wasPressed_ = pressed;

	bool typeMatched = false;
	switch (type_) {
	case ComboButtonInputType::kPressed: typeMatched = pressed; break;
	case ComboButtonInputType::kTriggered: typeMatched = triggered; break;
	case ComboButtonInputType::kReleased: typeMatched = released; break;
	case ComboButtonInputType::kPressTriggerReleased: typeMatched = pressed || triggered || released; break;
	case ComboButtonInputType::kPressTriggered: typeMatched = pressed || triggered; break;
	case ComboButtonInputType::kPressReleased: typeMatched = pressed || released; break;
	case ComboButtonInputType::kTriggerReleased: typeMatched = triggered || released; break;
	default: break;
	}
	if (!typeMatched || !MatchesComboDirection(direction_, ctx.worldStickDirection)) return false;

	// 離し入力では離した直前、その他では現在の保持時間を条件に使う。
	const float measuredHold = released ? releasedHoldSeconds_ : holdSeconds_;
	if (measuredHold < minHoldSeconds_ || (maxHoldSeconds_ > 0.0f && measuredHold > maxHoldSeconds_)) return false;
	const float chargeRatio = (std::min)(measuredHold / chargeSeconds_, 1.0f);
	return chargeRatio >= minChargeRatio_ && chargeRatio <= maxChargeRatio_;
}

#pragma region ComboSequence

/// <summary>
/// コンボボタンを順番に登録
/// </summary>
void Combo::ComboSequence::RegisterCombo(const std::vector<ComboButton>& buttons, bool sequential) {
	// 新しいコンボ入力列を登録するため、古い入力列を破棄する
	comboButtons_.clear();
	currentIndex_ = 0;
	sequential_ = sequential;
	for (auto& b : buttons) {
		// ボタン設定は値としてコピーして保持する
		comboButtons_.emplace_back(b);
	}
	for (const ComboButton& button : comboButtons_) {
		// 登録時点では前回の押下時間を持ち越さない。
		button.ResetState();
	}
}

/// <summary>
/// コンボ成立チェック
/// </summary>
	bool Combo::ComboSequence::Update(const Character::CharacterContext& ctx) {
		if (comboButtons_.empty()) return false;
		// ゲームパッド入力はIsInput内で安全に不成立となり、アクション入力は
		// 入力システムを持たないAIキャラクターでも評価できるようにする。
		if (sequential_) {
			// 順番入力では現在位置の入力だけを受け付ける。
			if (!comboButtons_[currentIndex_].IsInput(ctx)) return false;
			++currentIndex_;
			if (currentIndex_ >= comboButtons_.size()) {
				currentIndex_ = 0;
				return true;
			}
			return false;
		}
		// 旧データ互換として、順番指定がない場合はどれか1つの入力で成立させる。
		for (const ComboButton& button : comboButtons_) {
			if (button.IsInput(ctx)) return true;
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
