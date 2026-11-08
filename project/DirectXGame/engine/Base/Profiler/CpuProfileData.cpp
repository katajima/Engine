#include "CpuProfileData.h"

namespace Engine {

	double ProfileEvent::GetDurationMilliseconds() const {
		// 異常な終了時刻を受け取った場合でもUIの表示値を負にしない。
		if (endTime <= startTime) {
			return 0.0;
		}

		// マイクロ秒からミリ秒へ変換して、UIと統計処理で扱いやすくする。
		return static_cast<double>(endTime - startTime) / 1000.0;
	}

	double ProfileFrame::GetDurationMilliseconds() const {
		// フレームが未完了、または時計が逆転した場合は0ミリ秒を返す。
		if (endTime <= startTime) {
			return 0.0;
		}

		// マイクロ秒で保持しているフレーム時間をミリ秒へ変換する。
		return static_cast<double>(endTime - startTime) / 1000.0;
	}
}
