#pragma once
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <Windows.h>

#include <experimental\filesystem>

namespace fs = std::experimental::filesystem;

enum class MODE {EDIT,NAVMESH,END};

extern HWND g_hWnd;

extern bool		bEnableTerrainPicking;
extern HINSTANCE g_hInst;

namespace MapTool
{

}



using namespace MapTool;
using namespace std;
