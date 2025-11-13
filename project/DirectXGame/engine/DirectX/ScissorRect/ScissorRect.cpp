#include "ScissorRect.h"

#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/base/WinApp/WinApp.h"

void ScissorRect::Initialize(Command* command)
{
	command_ = command;	// コマンド
}

void ScissorRect::SettingScissorRect()
{
	int32_t width = WinApp::GetClientWidth(false);
	int32_t height = WinApp::GetClientHeight(false);

	// シザー矩形の設定
	D3D12_RECT scissorRect{};
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = width;
	scissorRect.top = 0;
	scissorRect.bottom = height;
	command_->GetList()->RSSetScissorRects(1, &scissorRect);
}
