#pragma once
#include <cstdint>

namespace Engine {
/// @brief 経過した発生周期の回数を取り出し、余り時間を保持する。
/// @param elapsedTime 未消費の経過秒数。
/// @param deltaTime 今回の経過秒数。0以下なら発生しない。
/// @param interval 発生間隔。0以下なら更新につき一回発生する。
/// @return 今回実行する発生回数。負荷上限を超える分は次回へ繰り越す。
uint32_t ConsumeEmissionBursts(float& elapsedTime, float deltaTime, float interval);
}
