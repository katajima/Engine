#pragma once

// リソースのリークチェッククラス
namespace Engine {
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
