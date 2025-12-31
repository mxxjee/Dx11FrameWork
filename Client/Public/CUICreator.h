#pragma once
#include "Client_Defines.h"

namespace Engine
{
	class CGameInstance;
}

namespace UICreator
{

	HRESULT		Create_HeartGroup(wstring LayerTag);

	HRESULT		Create_Interaction_UI(wstring LayerTag);
	HRESULT		Create_Interaction_TalkUI(wstring LayerTag);
	HRESULT		Create_Interaction_GetUI(wstring LayerTag);
	HRESULT		Create_Interaction_SeeUI(wstring LayerTag);
	HRESULT		Create_Interaction_OpenUI(wstring LayerTag);



	HRESULT		Create_NPC_Dialogue_UI(wstring LayerTag);
	HRESULT		Create_Loading_UI(wstring LayerTag);
	HRESULT		Create_ItemGet_UI(wstring LayerTag);
	HRESULT		Create_ItemGet_Desc_UI(wstring LayerTag);
	HRESULT		Create_See_Desc_UI(wstring LayerTag);


	HRESULT		Create_InvenSlot(wstring LayerTag);


	HRESULT		Create_InventorySceneLine(wstring LayerTag);
	HRESULT		Create_InventorySceneSlot(wstring LayerTag);

	extern CGameInstance* m_pGameInstance;
	
}