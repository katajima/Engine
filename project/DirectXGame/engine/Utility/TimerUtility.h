#pragma once
#include <chrono>
#include <string>
#include"DirectXGame/engine/base/Logger.h"

/// <summary>
/// デバック用タイマー
/// </summary>
class DebugTimer
{
public:
	/// <summary>
	/// 開始
	/// </summary>
	void StartTimer() {
		start = std::chrono::steady_clock::now();
	}
	/// <summary>
	/// 終了
	/// </summary>
	void EndTimer() {
		end = std::chrono::steady_clock::now();
	}

	/// <summary>
	/// ミリ秒取得
	/// </summary>
	float GetTimeMs() const {
		return std::chrono::duration<float, std::milli>(end - start).count();
	}

	/// <summary>
	/// 秒取得
	/// </summary>
	float GetTimeSec() const {
		return GetTimeMs() * 0.001f;
	}
	/// <summary>
	/// ログ出力
	/// </summary>
	void LogTimeMs(const std::string& message) const {
		Logger::Log(message + std::to_string(GetTimeMs()) + " ms" + "\n");
	}
	/// <summary>
	/// ログ出力
	/// </summary>
	void LogTimeMs(const std::string& message, const std::string& message2) const {
		Logger::Log(message + std::to_string(GetTimeMs()) + " ms " + message2 + "\n");
	}

	/// <summary>
	/// 秒ログ
	/// </summary>
	void LogTimeSec(const std::string& message) const {
		Logger::Log(message + std::to_string(GetTimeSec()) + " s\n");
	}

	/// <summary>
	/// 秒＋追加文字列ログ
	/// </summary>
	void LogTimeSec(const std::string& message, const std::string& message2) const {
		Logger::Log(message + std::to_string(GetTimeSec()) + " s " + message2 + "\n");
	}

private:
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point end;

};


