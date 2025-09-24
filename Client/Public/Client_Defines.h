#pragma once

#include <Windows.h>
#include <process.h>


#define PROTO_OBJ_NAME(name) L"Proto_GameObject_" + _wstring(name)
#define PROTO_COMPONENT_NAME(name) L"Proto_Component_" + _wstring(name)


extern HWND g_hWnd;
extern HINSTANCE g_hInst;

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;


	enum class LEVEL_ID {STATIC,LOGO,LOADING,GAMEPLAY,END};
	//enum class LEV
}


using namespace Client;
using namespace std;


#define		REGISTERLEVEL()