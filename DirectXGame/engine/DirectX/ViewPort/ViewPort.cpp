#include "ViewPort.h"
#include "DirectXGame/engine/DirectX/Command/Command.h"
#include "DirectXGame/engine/base/WinApp/WinApp.h"

void ViewPort::Initialize(Command* command)
{
	command_ = command;
}

void ViewPort::SettingViewport()
{
	// ビューポートの設定
	D3D12_VIEWPORT viewport{};


	FLOAT width = static_cast<FLOAT>(WinApp::GetClientWidth(false));
	FLOAT height = static_cast<FLOAT>(WinApp::GetClientHeight(false));

#ifndef _DEBUG
	//width = static_cast<FLOAT>(WinApp::GetClientWidth());
	//height = static_cast<FLOAT>(WinApp::GetClientHeight());
#endif // _DEBUG


	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = width;
	viewport.Height = height;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	command_->GetList()->RSSetViewports(1, &viewport);
}
