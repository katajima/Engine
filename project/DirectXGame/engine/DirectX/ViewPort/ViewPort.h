#pragma once

class Command;
class ViewPort
{
public:
	ViewPort() = default;
	~ViewPort() = default;

	// 初期化
	void Initialize(Command* command);

	// ビューポートを設定
	void SettingViewport();
private:
	// コマンドのインスタンスを受け取る箱
	Command* command_ = nullptr;
};

