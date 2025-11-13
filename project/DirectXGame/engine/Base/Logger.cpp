#include"Logger.h"
namespace Logger {
	// 出力に表示
	void Log(const std::string& message) {
		OutputDebugStringA(message.c_str());
	}
}