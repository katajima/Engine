#include "InputSystem.h"
#include "DirectXGame/engine/Math/MathFunctions.h"

namespace {
	// デジタルキーをスティック入力へ変換するときの最大入力値。
	constexpr float kDigitalInputMagnitude = 1.0f;
	// マウス移動量を視点入力へ変換する感度。
	constexpr float kMouseLookSensitivity = 0.05f;
	// ゲームパッドトリガーを押下として扱うしきい値。
	constexpr float kTriggerDeadZone = 0.2f;
	// 右クリックとして使用するマウスボタン番号。
	constexpr int kRightMouseButton = 1;
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
	if (input->IsPushKey(DIK_A)) keyboardMove.x -= kDigitalInputMagnitude;
	if (input->IsPushKey(DIK_D)) keyboardMove.x += kDigitalInputMagnitude;
	if (input->IsPushKey(DIK_W)) keyboardMove.y += kDigitalInputMagnitude;
	if (input->IsPushKey(DIK_S)) keyboardMove.y -= kDigitalInputMagnitude;

	// 左スティックとキーボード移動入力を合成
	Vector2 moveStick = input->GetGamePadLeftStick();
	moveStick.x += keyboardMove.x;
	moveStick.y += keyboardMove.y;
	playerInputData_.moveShick = Math::ClampLength(moveStick);

	// キーボード視点入力を作成
	Vector2 keyboardLook{};
	if (input->IsPushKey(DIK_LEFT)) keyboardLook.x -= kDigitalInputMagnitude;
	if (input->IsPushKey(DIK_RIGHT)) keyboardLook.x += kDigitalInputMagnitude;
	if (input->IsPushKey(DIK_UP)) keyboardLook.y += kDigitalInputMagnitude;
	if (input->IsPushKey(DIK_DOWN)) keyboardLook.y -= kDigitalInputMagnitude;

	// 右クリック中だけマウス移動を視点入力として扱う
	Vector2 mouseLook{};
	if (input->IsMousePressed(kRightMouseButton)) {
		const Vector2 mouseDelta = input->GetMouseMoveDelta();
		mouseLook.x = mouseDelta.x * kMouseLookSensitivity;
		mouseLook.y = -mouseDelta.y * kMouseLookSensitivity;
	}

	// 右スティック、矢印キー、マウス視点入力を合成
	Vector2 lookStick = input->GetGamePadRightStick();
	lookStick.x += keyboardLook.x + mouseLook.x;
	lookStick.y += keyboardLook.y + mouseLook.y;
	playerInputData_.lookStick = Math::ClampLength(lookStick);

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
	playerInputData_.dashHeld =
		lt > kTriggerDeadZone ||
		input->IsPushKey(DIK_LSHIFT);

	// ロックオン入力している間
	float rt = input->GetGamePadRightTrigger();
	playerInputData_.lockOnHeld =
		rt > kTriggerDeadZone ||
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
	// メニューではWASDと矢印キーのどちらでも項目を移動できるようにする。
	if (input->IsPushKey(DIK_A)) gameMove.x -= kDigitalInputMagnitude;
	if (input->IsPushKey(DIK_D)) gameMove.x += kDigitalInputMagnitude;
	if (input->IsPushKey(DIK_W)) gameMove.y += kDigitalInputMagnitude;
	if (input->IsPushKey(DIK_S)) gameMove.y -= kDigitalInputMagnitude;
	if (input->IsPushKey(DIK_LEFT)) gameMove.x -= kDigitalInputMagnitude;
	if (input->IsPushKey(DIK_RIGHT)) gameMove.x += kDigitalInputMagnitude;
	if (input->IsPushKey(DIK_UP)) gameMove.y += kDigitalInputMagnitude;
	if (input->IsPushKey(DIK_DOWN)) gameMove.y -= kDigitalInputMagnitude;
	gameInputData_.moveShick = Math::ClampLength(gameMove);
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
