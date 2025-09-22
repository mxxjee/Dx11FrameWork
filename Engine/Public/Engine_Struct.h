#ifndef Engine_Struct_h__
#define Engine_Struct_h__

namespace Engine
{

	typedef struct tagEngine_Desc
	{
		HWND hWnd;
		unsigned int iWinSizeX;
		unsigned int iWinSizeY;
		unsigned int iLevels;
		WINMODE		winMode;

	}ENGINE_DESC;
}


#endif // Engine_Struct_h__
