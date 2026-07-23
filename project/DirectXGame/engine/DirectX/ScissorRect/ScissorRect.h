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

		// 初期化
		void Initialize(Command* command);

		// シザー矩形を設定
		void SettingScissorRect();


	private:
		// コマンドのインスタンスを受け取る箱
		Command* command = nullptr;
	};
}
