#include "FrameTime.h"

#include <algorithm>
#include <cmath>

void Engine::FrameTime::Update(float unscaledDeltaTime, float timeScale) {
    mUnscaledDeltaTime = std::isfinite(unscaledDeltaTime) && unscaledDeltaTime > 0.0f
        ? unscaledDeltaTime : 0.0f;
    const float scale = std::isfinite(timeScale) ? (std::clamp)(timeScale, 0.0f, 4.0f) : 0.0f;
    mDeltaTime = mUnscaledDeltaTime * scale;
    mElapsedTime += mUnscaledDeltaTime;
}

float Engine::FrameTime::GetDeltaTime() const { return mDeltaTime; }

float Engine::FrameTime::GetUnscaledDeltaTime() const { return mUnscaledDeltaTime; }

double Engine::FrameTime::GetElapsedTime() const { return mElapsedTime; }
