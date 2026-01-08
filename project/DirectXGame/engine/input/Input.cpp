#include"Input.h"
#include <algorithm> // std::clamp
#include <cstring>   // memcpy
#include <cassert>

namespace
{
	Vector2 ApplyCircularDeadZone(
		SHORT rawX,
		SHORT rawY,
		SHORT deadZone)
	{
		Vector2 result{};

		const float fx = static_cast<float>(rawX);
		const float fy = static_cast<float>(rawY);

		const float length = std::sqrt(fx * fx + fy * fy);

		// デッドゾーン内
		if (length <= static_cast<float>(deadZone))
		{
			return result; // (0,0)
		}

		// 最大値（XInput の仕様）
		constexpr float MAX_VALUE = 32767.0f;

		// 正規化された方向
		const float nx = fx / length;
		const float ny = fy / length;

		// デッドゾーン外を 0〜1 に再スケール
		const float normalizedLength =
			((std::min)(length, MAX_VALUE) - deadZone) /
			(MAX_VALUE - deadZone);

		result.x = nx * normalizedLength;
		result.y = ny * normalizedLength;

		return result;
	}
}

void Engine::Input::Intialize(WinApp* winApp)
{
	HRESULT result;

	// 借りてきたWinAppのインスタンスを記録
	this->winApp_ = winApp;

	//　DirectInputのインスタンス生成
	result = DirectInput8Create(winApp_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));
	//排他制御レベルセット
	result = keyboard->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));


	// マウス入力の初期化
	result = directInput->CreateDevice(GUID_SysMouse, &mouseDevice_, NULL);
	assert(SUCCEEDED(result));

	result = mouseDevice_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(result));

	result = mouseDevice_->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(result));

	// XInput 初期化（安全のため）
	ZeroMemory(&xInputState_, sizeof(XINPUT_STATE));
	ZeroMemory(&preXInputState_, sizeof(XINPUT_STATE));
	leftTrigger_ = rightTrigger_ = 0.0f;
	prevLeftTrigger_ = prevRightTrigger_ = 0.0f;

}

void Engine::Input::Update()
{
	// -----------------------------
	// キーボード入力
	// -----------------------------
	memcpy(keyPre, key, sizeof(key));
	keyboard->Acquire();
	keyboard->GetDeviceState(sizeof(key), key);

	// -----------------------------
	// マウス
	// -----------------------------
	preMouse_ = mouse_;
	mouseDevice_->Acquire();
	mouseDevice_->GetDeviceState(sizeof(DIMOUSESTATE), &mouse_);


	// -----------------------------
	// Gamepad (XInput)
	// -----------------------------
	// 前フレームの状態を退避
	preXInputState_ = xInputState_;

	// 前フレームのトリガー値を退避（※leftTrigger_は前フレーム値）
	prevLeftTrigger_ = leftTrigger_;
	prevRightTrigger_ = rightTrigger_;

	// 新しい状態を取得
	XINPUT_STATE newState{};
	const DWORD result = XInputGetState(0, &newState);

	if (result == ERROR_SUCCESS)
	{
		xInputState_ = newState;
	}
	else
	{
		// 未接続：ゼロ扱いにして安全に
		ZeroMemory(&xInputState_, sizeof(XINPUT_STATE));
	}

	// 新しい状態からトリガーを更新（0..1）
	leftTrigger_ = static_cast<float>(xInputState_.Gamepad.bLeftTrigger) / 255.0f;
	rightTrigger_ = static_cast<float>(xInputState_.Gamepad.bRightTrigger) / 255.0f;

}

bool Engine::Input::IsPushKey(BYTE keyNumber) const
{
	return key[keyNumber] != 0;
}

bool Engine::Input::IsTriggerKey(BYTE keyNumber) const
{
	return (key[keyNumber] != 0) && (keyPre[keyNumber] == 0);
}

bool Engine::Input::IsKeyReleased(uint8_t _key) const
{
	return (key[_key] == 0) && (keyPre[_key] != 0);
}

bool Engine::Input::IsMouseTriggered(uint8_t _buttonNum) const
{
	return (mouse_.rgbButtons[_buttonNum] != 0) && (preMouse_.rgbButtons[_buttonNum] == 0);
}

bool Engine::Input::IsMousePressed(uint8_t _buttonNum) const
{
	return (mouse_.rgbButtons[_buttonNum] != 0);
}

bool Engine::Input::IsMouseReleased(uint8_t _buttonNum) const
{
	return (mouse_.rgbButtons[_buttonNum] == 0) && (preMouse_.rgbButtons[_buttonNum] != 0);
}

/// <summary>
/// マウスの位置を取得
/// </summary>
/// <returns></returns>
Vector2 Engine::Input::GetMousePosition() const
{
	POINT mousePos;
	GetCursorPos(&mousePos);

	ScreenToClient(winApp_->GetHwnd(), &mousePos);

	Vector2 result;
	result.x = static_cast<float>(mousePos.x);
	result.y = static_cast<float>(mousePos.y);

	return result;
}


// ゲームパッド
bool Engine::Input::IsGamePadTriggered(GamePadButton button) const
{
	const WORD mask = static_cast<WORD>(button);
	return (xInputState_.Gamepad.wButtons & mask) && !(preXInputState_.Gamepad.wButtons & mask);
}

bool Engine::Input::IsGamePadPressed(GamePadButton button) const
{
	const WORD mask = static_cast<WORD>(button);
	return (xInputState_.Gamepad.wButtons & mask) != 0;
}

bool Engine::Input::IsGamePadReleased(GamePadButton button) const
{
	const WORD mask = static_cast<WORD>(button);
	return !(xInputState_.Gamepad.wButtons & mask) && (preXInputState_.Gamepad.wButtons & mask);
}

Vector2 Engine::Input::GetGamePadLeftStick() const
{
	return ApplyCircularDeadZone(
		xInputState_.Gamepad.sThumbLX,
		xInputState_.Gamepad.sThumbLY,
		XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
}

Vector2 Engine::Input::GetGamePadRightStick() const
{
	return ApplyCircularDeadZone(
		xInputState_.Gamepad.sThumbRX,
		xInputState_.Gamepad.sThumbRY,
		XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
}

float Engine::Input::GetGamePadLeftTrigger() const
{
	return static_cast<float>(xInputState_.Gamepad.bLeftTrigger) / 255.0f;
}

float Engine::Input::GetGamePadRightTrigger() const
{
	return static_cast<float>(xInputState_.Gamepad.bRightTrigger) / 255.0f;
}




bool Engine::Input::IsLeftTriggerPressed(float threshold) const
{
	return leftTrigger_ >= threshold;
}

bool Engine::Input::IsRightTriggerPressed(float threshold) const
{
	return rightTrigger_ >= threshold;
}

bool Engine::Input::IsLeftTriggerTriggered(float threshold) const
{
	const bool now = (leftTrigger_ >= threshold);
	const bool prev = (prevLeftTrigger_ >= threshold);
	return now && !prev;
}

bool Engine::Input::IsRightTriggerTriggered(float threshold) const
{
	const bool now = (rightTrigger_ >= threshold);
	const bool prev = (prevRightTrigger_ >= threshold);
	return now && !prev;
}

bool Engine::Input::IsLeftTriggerReleased(float threshold) const
{
	const bool now = (leftTrigger_ >= threshold);
	const bool prev = (prevLeftTrigger_ >= threshold);
	return !now && prev;
}

bool Engine::Input::IsRightTriggerReleased(float threshold) const
{
	const bool now = (rightTrigger_ >= threshold);
	const bool prev = (prevRightTrigger_ >= threshold);
	return !now && prev;
}
