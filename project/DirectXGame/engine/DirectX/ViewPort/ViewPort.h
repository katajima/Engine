#pragma once


namespace Engine {
	// 前方宣言
	class Command;

	// ビューポート
/// <summary>
/// ViewPortを管理・実装するクラス。
/// </summary>
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
		Command* command = nullptr;
	};
}
