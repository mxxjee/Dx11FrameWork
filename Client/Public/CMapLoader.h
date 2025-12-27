#pragma once
#include "Client_Defines.h"
#include "CInteractionObject.h"


NS_BEGIN(Engine)
class CLayer;
NS_END
NS_BEGIN(Client)


namespace CMapLoader

{

	HRESULT		Load_Town();
	HRESULT		Load_Dungeon();


	//////////////각종 매니저에게전달하는함수들/////////////////
	void			Load_Terrain(const string& LoadPath);
	void			Load_NavMesh(LEVEL_ID LevelID,const string& LoadPath);
	void			Load_Interaction(string LevelName, const string& LoadPath);

	///////////////////
	HRESULT                 Make_Object_By_LoadData(string SceneName, CLayer* pLayer);
	
	
	
	
	
	/// <summary>
	/// /////////////////////////////////
	/// </summary>
	extern UMap<size_t, vector<DefaultInteractionData>> m_sceneInteractbles;


};

NS_END
