#pragma once
#include<string>
#include <windows.h>

// 文字コードユーティリティ
namespace StringUtility 
{
	//std::wstringからstd::stringへ変換する関数
	/// <summary>
	/// string->wstring
	/// </summary>
	std::wstring ConvertString(const std::string& str);
	/// <summary>
	/// wstring->string
	/// </summary>
	std::string ConvertString(const std::wstring& str);


}

