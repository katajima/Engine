#include "InputManager.h"
#include <cmath>



void InputManager::Intialize(Engine::Input* input){
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

    // ---- Look（RightStick だけ例）----
    value2_[Idx(Action::Look)] = input_->GetGamePadRightStick();

    // ---- Attack（Pad B）----
    triggered_[Idx(Action::LightAttack)] =
        input_->IsGamePadTriggered(GamePadButton::GAMEPAD_B);

    pressed_[Idx(Action::LightAttack)] =
        input_->IsGamePadPressed(GamePadButton::GAMEPAD_B);

    released_[Idx(Action::LightAttack)] =
        input_->IsGamePadReleased(GamePadButton::GAMEPAD_B);

    // ---- Attack（Pad X）----
    triggered_[Idx(Action::HeavyAttack)] =
        input_->IsGamePadTriggered(GamePadButton::GAMEPAD_X);

    pressed_[Idx(Action::HeavyAttack)] =
        input_->IsGamePadPressed(GamePadButton::GAMEPAD_X);

    released_[Idx(Action::HeavyAttack)] =
        input_->IsGamePadReleased(GamePadButton::GAMEPAD_X);

    // ---- Attack（Pad A）----
    triggered_[Idx(Action::Jump)] =
        input_->IsGamePadTriggered(GamePadButton::GAMEPAD_A);

    pressed_[Idx(Action::Jump)] =
        input_->IsGamePadPressed(GamePadButton::GAMEPAD_A);

    released_[Idx(Action::Jump)] =
        input_->IsGamePadReleased(GamePadButton::GAMEPAD_A);

    // ---- Attack（Pad RB）----
    triggered_[Idx(Action::Special)] =
        input_->IsGamePadTriggered(GamePadButton::GAMEPAD_RB);

    pressed_[Idx(Action::Special)] =
        input_->IsGamePadPressed(GamePadButton::GAMEPAD_RB);

    released_[Idx(Action::Special)] =
        input_->IsGamePadReleased(GamePadButton::GAMEPAD_RB);


   

    // ---- Dash（LT 押した瞬間）----
    triggered_[Idx(Action::Dash)] = input_->IsLeftTriggerTriggered(0.5f);
    pressed_[Idx(Action::Dash)] = input_->IsLeftTriggerPressed(0.5f);
    released_[Idx(Action::Dash)] = input_->IsLeftTriggerReleased(0.5f);
    value1_[Idx(Action::Dash)] = input_->GetGamePadLeftTrigger(); // アナログ値も欲しい場合

   
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
