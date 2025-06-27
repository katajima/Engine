#pragma once
#include <chrono>
#include <string>
#include"DirectXGame/engine/base/Logger.h"

class DebugTimer
{
public:
	void StartTimer() {
		start = std::chrono::steady_clock::now();
	}
	void EndTimer() {
		end = std::chrono::steady_clock::now();
	}

	// ミリ秒取得
	float GetTimeMs() const {
		return std::chrono::duration<float, std::milli>(end - start).count();
	}

	// 秒取得
	float GetTimeSec() const {
		return GetTimeMs() * 0.001f;
	}

	void LogTimeMs(const std::string& message) const {
		Logger::Log(message + std::to_string(GetTimeMs()) + " ms" + "\n");
	}
	void LogTimeMs(const std::string& message, const std::string& message2) const {
		Logger::Log(message + std::to_string(GetTimeMs()) + " ms " + message2 + "\n");
	}

	// 秒ログ
	void LogTimeSec(const std::string& message) const {
		Logger::Log(message + std::to_string(GetTimeSec()) + " s\n");
	}

	// 秒＋追加文字列ログ
	void LogTimeSec(const std::string& message, const std::string& message2) const {
		Logger::Log(message + std::to_string(GetTimeSec()) + " s " + message2 + "\n");
	}

private:
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point end;

};


