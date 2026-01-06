#pragma once
#include "Client_Defines.h"

namespace Engine
{
	class CGameInstance;

}

namespace Client
{
	class CLoadingUI;
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
	HRESULT		Create_Loading_UI(wstring LayerTag,vector<CLoadingUI*>& UIVec);
	HRESULT		Create_ItemGet_UI(wstring LayerTag);
	HRESULT		Create_ItemGet_Desc_UI(wstring LayerTag);
	HRESULT		Create_See_Desc_UI(wstring LayerTag);
	HRESULT		Create_EmptySlot_UI(wstring LayerTag);


	HRESULT		Create_InvenSlot(wstring LayerTag);


	/// /////inventory SCene
	HRESULT		Create_InventorySceneLine(wstring LayerTag);
	HRESULT		Create_InventoryItemInfo(wstring LayerTag);
	HRESULT		Create_InventoryCursor(wstring LayerTag);
	HRESULT		Create_InventorySceneSlot(wstring LayerTag);


	///Level_UI
	HRESULT		Create_LevelUI(wstring LayerTag);

	extern CGameInstance* m_pGameInstance;
	
}