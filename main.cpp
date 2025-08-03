#include<Windows.h>
#include"winuser.h"

#include"DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Base/pch.h"


#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#include <crtdbg.h>


//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	D3DResourceLeakchecker leakCheck;
	_CrtSetDbgFlag(
		_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF
	);

	Framework* game = new MyGame();

	game->Run();

	delete game;
	
	return 0;
}
