#pragma once


namespace Engine {
/// @brief 一つの実行ループが所有する実時間とゲーム時間を保持する。
class FrameTime {
public:
    /// @brief 実測時間と倍率から今回の更新時間を計算する。
    /// @param unscaledDeltaTime 前フレームから経過した実時間（秒）。
    /// @param timeScale ゲーム時間倍率（0〜4）。
    void Update(float unscaledDeltaTime, float timeScale);

    /// @brief 時間倍率を反映した更新時間を取得する。
    /// @return ゲーム内の経過秒数。
    float GetDeltaTime() const;
    /// @brief 時間倍率を反映しない更新時間を取得する。
    /// @return 実時間の経過秒数。
    float GetUnscaledDeltaTime() const;
    /// @brief 計測開始以降の実時間を取得する。
    /// @return 累積した実時間（秒）。
    double GetElapsedTime() const;

private:
    float mDeltaTime = 0.0f;
    float mUnscaledDeltaTime = 0.0f;
    double mElapsedTime = 0.0;
};
}
