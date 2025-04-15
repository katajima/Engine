#pragma once
#include"DirectXGame/engine/DirectX/Common/DirectXCommon.h"


class Command;
class DXGIDevice;
class SrvManager;
// 3Dモデル共通部分
class ModelCommon
{
public:
	// 初期化
	void Initialize(Command* command, DXGIDevice* DXGIDevice, SrvManager* srvManager);

	
	Command* GetCommand() { return command_; }
	DXGIDevice* GetDXGIDevice() { return DXGIDevice_; };
	SrvManager* GetSrvManager() { return srvManager_; };

private:
	Command* command_;
	DXGIDevice* DXGIDevice_;
	SrvManager* srvManager_;
};

