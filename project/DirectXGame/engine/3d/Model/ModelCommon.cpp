#include"ModelCommon.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"

void Engine::ModelCommon::Initialize(Command* command, DXGIDevice* dxgiDevice, SrvManager* srvManager)
{
    this->dxgiDevice = dxgiDevice;   // デバイス
    this->command = command;         // コマンド
    this->srvManager = srvManager;   // SRV管理クラス
}
