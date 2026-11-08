#pragma once

#include "CpuProfiler.h"

#include <string_view>

namespace Engine {

	/// <summary>
	/// スコープの生成と破棄を利用してCPU計測を自動終了するRAIIクラス。
	/// </summary>
	class ScopedCpuTimer {
	public:
		/// <summary>
		/// 指定名のCPUスコープを開始する。
		/// </summary>
		explicit ScopedCpuTimer(std::string_view name, CpuProfiler& profiler = CpuProfiler::Get());

		/// <summary>
		/// 対応するCPUスコープを終了する。
		/// </summary>
		~ScopedCpuTimer();

		// コピーによる二重終了を防止する。
		ScopedCpuTimer(const ScopedCpuTimer&) = delete;
		ScopedCpuTimer& operator=(const ScopedCpuTimer&) = delete;

	private:
		// 計測終了を通知する対象プロファイラ。
		CpuProfiler* profiler_ = nullptr;
	};
}

// 同じ関数内で複数のスコープを置けるよう、行番号を変数名に利用する。
#define ENGINE_CPU_PROFILE_SCOPE(name) ::Engine::ScopedCpuTimer cpuProfileScope##__LINE__(name)
