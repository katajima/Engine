#pragma once
#include<string>
#include<dxgidebug.h>
#include<Windows.h>
#include<format>
namespace Logger
{
	/// <summary>
	/// 出力ウィンドウを出す
	/// </summary>
	void Log(const std::string& message);
}
