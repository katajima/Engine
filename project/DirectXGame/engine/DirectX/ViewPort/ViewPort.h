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

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Command* command);

		/// <summary>
		/// ビューポートを設定
		/// </summary>
		void SettingViewport();
	private:
		// コマンドのインスタンスを受け取る箱
		Command* command = nullptr;
	};
}
