#include "ScopedCpuTimer.h"

namespace Engine {

	ScopedCpuTimer::ScopedCpuTimer(std::string_view name, CpuProfiler& profiler) : profiler_(&profiler) {
		// 計測の有効状態にかかわらずBeginScopeを呼び、TLSの対応関係を揃える。
		profiler_->BeginScope(name);
	}

	ScopedCpuTimer::~ScopedCpuTimer() {
		// 生成時に保存したプロファイラへ、スコープの終了を通知する。
		if (profiler_ != nullptr) {
			profiler_->EndScope();
		}
	}
}
