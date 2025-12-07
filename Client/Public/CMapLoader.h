#pragma once
#include "Client_Defines.h"

NS_BEGIN(Client)
namespace CMapLoader

{

	HRESULT		Load_Town();


	//////////////각종 매니저에게전달하는함수들/////////////////
	void			Load_Terrain(const string& LoadPath);
	void			Load_NavMesh(LEVEL_ID LevelID,const string& LoadPath);



};

NS_END
