
namespace Easing {

    // イーズイン
    template <typename T>
    static T EaseIn(const T& start, const T& end, float t) {
        if (t > 1.0f) t = 1.0f;
        return start + (end - start) * t * t;
    }

    // イーズアウト
    template <typename T>
    static T EaseOut(const T& start, const T& end, float t) {
        if (t > 1.0f) t = 1.0f;
        return start + (end - start) * (1 - (1 - t) * (1 - t));
    }

    // イーズインアウト
    template <typename T>
    static T EaseInOut(const T& start, const T& end, float t) {
        /// <summary>
        /// t を 0.0～1.0 の範囲に制限
        /// </summary>
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;

        /// <summary>
        /// Ease-In-Out 補間
        /// </summary>
        if (t < 0.5f) {
            t = 2.0f * t * t; // Ease-In 部分
        }
        else {
            t = -2.0f * (t - 1.0f) * (t - 1.0f) + 1.0f; // Ease-Out 部分
        }

        /// <summary>
        /// 補間計算
        /// </summary>
        return start + (end - start) * static_cast<T>(t);
    }

}
