#include "ViewPort.h"
#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/base/WinApp/WinApp.h"

void Engine::ViewPort::Initialize(Command* command)
{
	command_ = command;	// コマンド
}

void Engine::ViewPort::SettingViewport()
{
	// ビューポートの設定
	D3D12_VIEWPORT viewport{};


	FLOAT width = static_cast<FLOAT>(WinApp::GetClientWidth(false));
	FLOAT height = static_cast<FLOAT>(WinApp::GetClientHeight(false));


	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = width;
	viewport.Height = height;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	command_->GetList()->RSSetViewports(1, &viewport);
}
