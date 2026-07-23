#pragma once

namespace Engine {
	// 前方宣言
	class Command;

	// シザーレクトクラス
/// <summary>
/// ScissorRectを管理・実装するクラス。
/// </summary>
	class ScissorRect
	{
	public:
		ScissorRect() = default;
		~ScissorRect() = default;

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Command* command);

		/// <summary>
		/// シザー矩形を設定
		/// </summary>
		void SettingScissorRect();


	private:
		// コマンドのインスタンスを受け取る箱
		Command* command = nullptr;
	};
}
