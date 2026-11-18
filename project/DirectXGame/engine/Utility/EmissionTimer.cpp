#include "EmissionTimer.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr uint32_t kMaxEmissionBurstsPerUpdate = 1024;
}

uint32_t Engine::ConsumeEmissionBursts(float& elapsedTime, float deltaTime, float interval) {
    if (!std::isfinite(deltaTime) || deltaTime <= 0.0f || !std::isfinite(interval)) {
        return 0;
    }
    if (interval <= 0.0f) {
        elapsedTime = 0.0f;
        return 1;
    }
    const double previous = std::isfinite(elapsedTime) ? (std::max)(elapsedTime, 0.0f) : 0.0;
    const double accumulated = previous + deltaTime;
    const double intervals = std::floor(accumulated / interval + 0.000001);
    const auto count = static_cast<uint32_t>((std::min)(intervals, static_cast<double>(kMaxEmissionBurstsPerUpdate)));
    elapsedTime = static_cast<float>((std::max)(0.0, accumulated - count * static_cast<double>(interval)));
    return count;
}
