#include "InputManager.h"
#include <cmath>



void InputManager::Initialize(Engine::Input* input){
	input_ = input;
}

void InputManager::Update(float dt) {
    frameCounter_++;
    ClearFrame();
    
    if (!input_) return;

    switch (context_)
    {
    case Context::Gameplay:
        BuildGameplayActions();
        break;
    case Context::UI:
        BuildUIActions();
        break;
    case Context::Pause:
        // 必要なら
        BuildUIActions();
        break;
    }
    RecordBufferedEvents();
}

void InputManager::ClearFrame(){
    for (int i = 0; i < Idx(Action::Max); ++i)
    {
        pressed_[i] = triggered_[i] = released_[i] = false;
        value2_[i] = Vector2{ 0,0 };
        value1_[i] = 0.0f;
    }
}

void InputManager::BuildGameplayActions(){
    // ---- Move（WASD + LeftStick 合成）----
    Vector2 move{ 0,0 };

    if (input_->IsPushKey(DIK_A)) move.x -= 1.0f;
    if (input_->IsPushKey(DIK_D)) move.x += 1.0f;
    if (input_->IsPushKey(DIK_W)) move.y += 1.0f;
    if (input_->IsPushKey(DIK_S)) move.y -= 1.0f;

    const Vector2 ls = input_->GetGamePadLeftStick();
    move.x += ls.x;
    move.y += ls.y;

    move = ClampLen1(move);
    value2_[Idx(Action::Move)] = move;
    pressed_[Idx(Action::Move)] = move.x != 0.0f || move.y != 0.0f;
    triggered_[Idx(Action::Move)] = pressed_[Idx(Action::Move)] && !isDown_[Idx(Action::Move)];
    released_[Idx(Action::Move)] = !pressed_[Idx(Action::Move)] && isDown_[Idx(Action::Move)];

    // ---- Look（RightStick + 矢印キー + 右クリック中のマウス移動）----
    Vector2 look = input_->GetGamePadRightStick();
    if (input_->IsPushKey(DIK_LEFT)) look.x -= 1.0f;
    if (input_->IsPushKey(DIK_RIGHT)) look.x += 1.0f;
    if (input_->IsPushKey(DIK_UP)) look.y += 1.0f;
    if (input_->IsPushKey(DIK_DOWN)) look.y -= 1.0f;
    if (input_->IsMousePressed(1)) {
        const Vector2 mouseDelta = input_->GetMouseMoveDelta();
        look.x += mouseDelta.x * 0.05f;
        look.y -= mouseDelta.y * 0.05f;
    }
    look = ClampLen1(look);
    value2_[Idx(Action::Look)] = look;
    pressed_[Idx(Action::Look)] = look.x != 0.0f || look.y != 0.0f;
    triggered_[Idx(Action::Look)] = pressed_[Idx(Action::Look)] && !isDown_[Idx(Action::Look)];
    released_[Idx(Action::Look)] = !pressed_[Idx(Action::Look)] && isDown_[Idx(Action::Look)];

    // ---- Attack（Pad X / 左クリック / J）----
    triggered_[Idx(Action::LightAttack)] =
        input_->IsGamePadTriggered(GamePadButton::GAMEPAD_X) ||
        input_->IsMouseTriggered(0) ||
        input_->IsTriggerKey(DIK_J);

    pressed_[Idx(Action::LightAttack)] =
        input_->IsGamePadPressed(GamePadButton::GAMEPAD_X) ||
        input_->IsMousePressed(0) ||
        input_->IsPushKey(DIK_J);

    released_[Idx(Action::LightAttack)] =
        input_->IsGamePadReleased(GamePadButton::GAMEPAD_X) ||
        input_->IsMouseReleased(0) ||
        input_->IsKeyReleased(DIK_J);

    // ---- Attack（Pad Y / K）----
    triggered_[Idx(Action::HeavyAttack)] =
        input_->IsGamePadTriggered(GamePadButton::GAMEPAD_Y) ||
        input_->IsTriggerKey(DIK_K);

    pressed_[Idx(Action::HeavyAttack)] =
        input_->IsGamePadPressed(GamePadButton::GAMEPAD_Y) ||
        input_->IsPushKey(DIK_K);

    released_[Idx(Action::HeavyAttack)] =
        input_->IsGamePadReleased(GamePadButton::GAMEPAD_Y) ||
        input_->IsKeyReleased(DIK_K);

    // ---- Jump（Pad A / Space）----
    triggered_[Idx(Action::Jump)] =
        input_->IsGamePadTriggered(GamePadButton::GAMEPAD_A) ||
        input_->IsTriggerKey(DIK_SPACE);

    pressed_[Idx(Action::Jump)] =
        input_->IsGamePadPressed(GamePadButton::GAMEPAD_A) ||
        input_->IsPushKey(DIK_SPACE);

    released_[Idx(Action::Jump)] =
        input_->IsGamePadReleased(GamePadButton::GAMEPAD_A) ||
        input_->IsKeyReleased(DIK_SPACE);

    // ---- Special（Pad RB / R）----
    triggered_[Idx(Action::Special)] =
        input_->IsGamePadTriggered(GamePadButton::GAMEPAD_RB) ||
        input_->IsTriggerKey(DIK_R);

    pressed_[Idx(Action::Special)] =
        input_->IsGamePadPressed(GamePadButton::GAMEPAD_RB) ||
        input_->IsPushKey(DIK_R);

    released_[Idx(Action::Special)] =
        input_->IsGamePadReleased(GamePadButton::GAMEPAD_RB) ||
        input_->IsKeyReleased(DIK_R);

    // ---- Skill（Pad B / E）----
    triggered_[Idx(Action::Skill)] =
        input_->IsGamePadTriggered(GamePadButton::GAMEPAD_B) ||
        input_->IsTriggerKey(DIK_E);

    pressed_[Idx(Action::Skill)] =
        input_->IsGamePadPressed(GamePadButton::GAMEPAD_B) ||
        input_->IsPushKey(DIK_E);

    released_[Idx(Action::Skill)] =
        input_->IsGamePadReleased(GamePadButton::GAMEPAD_B) ||
        input_->IsKeyReleased(DIK_E);

    // ---- Dodge（Pad LB / LeftShift）----
    triggered_[Idx(Action::Dodge)] =
        input_->IsGamePadTriggered(GamePadButton::GAMEPAD_LB) ||
        input_->IsTriggerKey(DIK_LSHIFT);

    pressed_[Idx(Action::Dodge)] =
        input_->IsGamePadPressed(GamePadButton::GAMEPAD_LB) ||
        input_->IsPushKey(DIK_LSHIFT);

    released_[Idx(Action::Dodge)] =
        input_->IsGamePadReleased(GamePadButton::GAMEPAD_LB) ||
        input_->IsKeyReleased(DIK_LSHIFT);

    // ---- Dash（LT / LeftShift）----
    triggered_[Idx(Action::Dash)] = input_->IsLeftTriggerTriggered(0.5f) || input_->IsTriggerKey(DIK_LSHIFT);
    pressed_[Idx(Action::Dash)] = input_->IsLeftTriggerPressed(0.5f) || input_->IsPushKey(DIK_LSHIFT);
    released_[Idx(Action::Dash)] = input_->IsLeftTriggerReleased(0.5f) || input_->IsKeyReleased(DIK_LSHIFT);
    value1_[Idx(Action::Dash)] = input_->GetGamePadLeftTrigger(); // アナログ値も欲しい場合

    // ---- LockOn（RT / Q）----
    triggered_[Idx(Action::LockOn)] = input_->IsRightTriggerTriggered(0.5f) || input_->IsTriggerKey(DIK_Q);
    pressed_[Idx(Action::LockOn)] = input_->IsRightTriggerPressed(0.5f) || input_->IsPushKey(DIK_Q);
    released_[Idx(Action::LockOn)] = input_->IsRightTriggerReleased(0.5f) || input_->IsKeyReleased(DIK_Q);
    value1_[Idx(Action::LockOn)] = input_->GetGamePadRightTrigger(); // ロックオン入力の押し込み量

   
    // ---- Pause（Esc or Start）----
    triggered_[Idx(Action::Pause)] =
        input_->IsTriggerKey(DIK_ESCAPE) ||
        input_->IsGamePadTriggered(GamePadButton::GAMEPAD_Start);
}

void InputManager::BuildUIActions(){
}

Vector2 InputManager::ClampLen1(const Vector2& v){
    const float lenSq = v.x * v.x + v.y * v.y;
    if (lenSq <= 1.0f) return v;

    const float len = std::sqrt(lenSq);
    if (len <= 1e-6f) return Vector2{ 0,0 };

    return Vector2{ v.x / len, v.y / len };
}

void InputManager::RecordBufferedEvents(){
    for (int i = 0; i < Idx(Action::Max); ++i)
    {
        const Action a = static_cast<Action>(i);

        if (Triggered(a))
        {
            isDown_[i] = true;
            pressFrame_[i] = frameCounter_;

            PushEvent(a, InputEvent::Type::Press, Value1(a), Value2(a));
        }
        if (Released(a))
        {
            isDown_[i] = false;
            PushEvent(a, InputEvent::Type::Release, Value1(a), Value2(a));
        }
    }
}
