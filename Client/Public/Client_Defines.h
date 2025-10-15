#pragma once

#include <Windows.h>
#include <process.h>
#include <cstring>

#include "SimpleMath/SimpleMath.h"
#include "MathUtils.h"


extern HWND g_hWnd;
extern HINSTANCE g_hInst;

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;


	enum class LEVEL_ID {STATIC,LOGO,LOADING,GAMEPLAY,UI,MAPTOOL,END};
	enum class RENDERGROUP {PRIORITY,NONALPHA,ALPHA, WORLD_UI_MINIMAP,UI,END};
	//enum class LEV
}


using namespace Client;
using namespace std;


#define		REGISTERLEVEL()