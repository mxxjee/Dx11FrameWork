#pragma once

#include <Windows.h>
#include <process.h>



extern HWND g_hWnd;
extern HINSTANCE g_hInst;

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;


	enum class LEVEL_ID {STATIC,LOGO,LOADING,GAMEPLAY,END};
}


using namespace Client;
using namespace std;
