#include"ModelCommon.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"

void ModelCommon::Initialize(Command* command, DXGIDevice* DXGIDevice, SrvManager* srvManager)
{
    DXGIDevice_ = DXGIDevice;   // デバイス
    command_ = command;         // コマンド
    srvManager_ = srvManager;   // SRV管理クラス
}
