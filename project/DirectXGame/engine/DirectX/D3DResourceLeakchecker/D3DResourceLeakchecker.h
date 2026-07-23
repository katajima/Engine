#pragma once

// リソースのリークチェッククラス
namespace Engine {
/// <summary>
/// D3DResourceLeakcheckerを管理・実装するクラス。
/// </summary>
	class D3DResourceLeakchecker
	{
	public:
		/// <summary>
		/// DirectX関連リソースの生存状況を出力
		/// </summary>
		static void ReportLiveObjects();

		/// <summary>
		/// デストラクタ
		/// </summary>
		~D3DResourceLeakchecker();
	};
}
