#include "ScissorRect.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/base/WinApp.h"

void ScissorRect::Initialize(Command* command)
{
	command_ = command;
}

void ScissorRect::SettingScissorRect()
{
	// シザー矩形の設定
	D3D12_RECT scissorRect{};
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = WinApp::kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = WinApp::kClientHeight;
	command_->GetList()->RSSetScissorRects(1, &scissorRect);
}
