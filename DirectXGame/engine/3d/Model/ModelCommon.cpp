#include"ModelCommon.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"

void ModelCommon::Initialize(Command* command, DXGIDevice* DXGIDevice, SrvManager* srvManager)
{
    DXGIDevice_ = DXGIDevice;
    command_ = command;
    srvManager_ = srvManager;
}
