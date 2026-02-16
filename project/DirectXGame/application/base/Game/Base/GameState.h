#pragma once
#include "GameEvent.h"

/// <summary>
/// ゲームステート
/// </summary>
class IGameState {
public:
    virtual ~IGameState() = default;
    virtual void OnEnter() {}
    virtual void OnExit() {}
    virtual void Update(float dt) = 0;
private:

};
