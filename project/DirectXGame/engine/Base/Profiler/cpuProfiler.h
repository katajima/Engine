#pragma once
#include <cstdint>

namespace Engine {


	class ImGuiManager;

	/// <summary>
	/// 
	/// </summary>
	struct CpuProfileEvent
	{
		const char* name = nullptr;

		uint64_t startTime = 0;
		uint64_t endTime = 0;

		uint32_t threadId = 0;
		uint32_t depth = 0;
		uint32_t parentIndex = UINT32_MAX;
	};

	/// <summary>
	/// CPU のプロファイリングを行うクラス。コンストラクタで初期化し、デストラクタでリソースを解放する。
	/// </summary>
	class CPUProfiler
	{
	public:
		CPUProfiler();
		~CPUProfiler();

		/// <summary>
		/// 新しいフレームの開始処理を行う。
		/// </summary>
		void BeginFrame();


		void EndFrame();
		
		void BeginScope();

		void EndScope();

	private:

	};
}

