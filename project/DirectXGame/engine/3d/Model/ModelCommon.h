#pragma once
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"


namespace Engine {
	// 前方宣言
	class Command;
	class DXGIDevice;
	class SrvManager;

	// 3Dモデル共通部分
/// <summary>
/// ModelCommonを管理・実装するクラス。
/// </summary>
	class ModelCommon
	{
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Command* command, DXGIDevice* DXGIDevice, SrvManager* srvManager);

		/// <summary>
		/// コマンド取得
		/// </summary>
		Command* GetCommand() { return command; }
		/// <summary>
		/// デバイス取得
		/// </summary>
		DXGIDevice* GetDXGIDevice() { return dxgiDevice; };
		/// <summary>
		/// Srvマネージャー取得
		/// </summary>
		SrvManager* GetSrvManager() { return srvManager; };
	private:
		Command* command;
		DXGIDevice* dxgiDevice;
		SrvManager* srvManager;
	};
}
