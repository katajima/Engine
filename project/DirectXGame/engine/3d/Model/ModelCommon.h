#pragma once
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"


namespace Engine {
	// 前方宣言
	class Command;
	class DXGIDevice;
	class SrvManager;

	// 3Dモデル共通部分
	class ModelCommon
	{
	public:
		// 初期化
		void Initialize(Command* command, DXGIDevice* DXGIDevice, SrvManager* srvManager);

		// コマンド取得
		Command* GetCommand() { return command; }
		// デバイス取得
		DXGIDevice* GetDXGIDevice() { return dxgiDevice; };
		// Srvマネージャー取得
		SrvManager* GetSrvManager() { return srvManager; };
	private:
		Command* command;
		DXGIDevice* dxgiDevice;
		SrvManager* srvManager;
	};
}
