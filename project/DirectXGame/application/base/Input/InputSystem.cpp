#include "InputSystem.h"
#include <cmath>

namespace {
	// ベクトルの長さを1以下にして斜め入力だけ速くならないようにする
	Vector2 ClampLen1(const Vector2& value) {
		const float lengthSq = value.x * value.x + value.y * value.y;
		if (lengthSq <= 1.0f) {
			return value;
		}

		const float length = std::sqrt(lengthSq);
		if (length <= 1e-6f) {
			return Vector2{};
		}

		return Vector2{ value.x / length, value.y / length };
	}
}

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

	// キーボード移動入力を作成
	Vector2 keyboardMove{};
	if (input->IsPushKey(DIK_A)) keyboardMove.x -= 1.0f;
	if (input->IsPushKey(DIK_D)) keyboardMove.x += 1.0f;
	if (input->IsPushKey(DIK_W)) keyboardMove.y += 1.0f;
	if (input->IsPushKey(DIK_S)) keyboardMove.y -= 1.0f;

	// 左スティックとキーボード移動入力を合成
	Vector2 moveStick = input->GetGamePadLeftStick();
	moveStick.x += keyboardMove.x;
	moveStick.y += keyboardMove.y;
	playerInputData_.moveShick = ClampLen1(moveStick);

	// キーボード視点入力を作成
	Vector2 keyboardLook{};
	if (input->IsPushKey(DIK_LEFT)) keyboardLook.x -= 1.0f;
	if (input->IsPushKey(DIK_RIGHT)) keyboardLook.x += 1.0f;
	if (input->IsPushKey(DIK_UP)) keyboardLook.y += 1.0f;
	if (input->IsPushKey(DIK_DOWN)) keyboardLook.y -= 1.0f;

	// 右クリック中だけマウス移動を視点入力として扱う
	Vector2 mouseLook{};
	if (input->IsMousePressed(1)) {
		const Vector2 mouseDelta = input->GetMouseMoveDelta();
		mouseLook.x = mouseDelta.x * 0.05f;
		mouseLook.y = -mouseDelta.y * 0.05f;
	}

	// 右スティック、矢印キー、マウス視点入力を合成
	Vector2 lookStick = input->GetGamePadRightStick();
	lookStick.x += keyboardLook.x + mouseLook.x;
	lookStick.y += keyboardLook.y + mouseLook.y;
	playerInputData_.lookStick = ClampLen1(lookStick);

	// ジャンプ入力押している間
	playerInputData_.jumpPressed =
		input->IsGamePadPressed(GamePadButton::GAMEPAD_A) ||
		input->IsPushKey(DIK_SPACE);
	// ジャンプ入力押した瞬間
	playerInputData_.jumpTrigger =
		input->IsGamePadTriggered(GamePadButton::GAMEPAD_A) ||
		input->IsTriggerKey(DIK_SPACE);

	// 回避入力押した瞬間
	playerInputData_.dodgeTrigger =
		input->IsGamePadTriggered(GamePadButton::GAMEPAD_LB) ||
		input->IsTriggerKey(DIK_LSHIFT);

	// ダッシュ入力している間
	float lt = input->GetGamePadLeftTrigger();
	const float triggerDeadZone = 0.2f;
	playerInputData_.dashHeld =
		lt > triggerDeadZone ||
		input->IsPushKey(DIK_LSHIFT);

	// ロックオン入力している間
	float rt = input->GetGamePadRightTrigger();
	playerInputData_.lockOnHeld =
		rt > triggerDeadZone ||
		input->IsPushKey(DIK_Q);

	// スキル入力押した瞬間
	playerInputData_.skillTrigger =
		input->IsGamePadTriggered(GamePadButton::GAMEPAD_B) ||
		input->IsTriggerKey(DIK_E);

	// 必殺技入力押した瞬間
	playerInputData_.specialTrigger =
		input->IsGamePadTriggered(GamePadButton::GAMEPAD_RB) ||
		input->IsTriggerKey(DIK_R);

}

void InputSystem::GameInputUpdate(float dt) {
	// ゲームUI用の移動入力を作成
	Vector2 gameMove = input->GetGamePadLeftStick();
	if (input->IsPushKey(DIK_A)) gameMove.x -= 1.0f;
	if (input->IsPushKey(DIK_D)) gameMove.x += 1.0f;
	if (input->IsPushKey(DIK_W)) gameMove.y += 1.0f;
	if (input->IsPushKey(DIK_S)) gameMove.y -= 1.0f;
	gameInputData_.moveShick = ClampLen1(gameMove);
	// 決定入力押した瞬間
	gameInputData_.decisionTrigger =
		input->IsGamePadTriggered(GamePadButton::GAMEPAD_B) ||
		input->IsTriggerKey(DIK_RETURN) ||
		input->IsTriggerKey(DIK_SPACE);
	// ポーズ入力押した瞬間
	gameInputData_.pauseTrigger =
		input->IsGamePadTriggered(GamePadButton::GAMEPAD_Back) ||
		input->IsTriggerKey(DIK_ESCAPE);
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
