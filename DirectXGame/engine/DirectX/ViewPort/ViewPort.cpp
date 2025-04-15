#include "ViewPort.h"
#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/base/WinApp.h"

void ViewPort::Initialize(Command* command)
{
	command_ = command;
}

void ViewPort::SettingViewport()
{
	// ビューポートの設定
	D3D12_VIEWPORT viewport{};
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = WinApp::kClientWidth;
	viewport.Height = WinApp::kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	command_->GetList()->RSSetViewports(1, &viewport);
}
