#include "CUI_Window_Inventory.h"
#include "CInventorySlot.h"
#include "CInput_Manager.h"
#include "MathUtils.h"
#include "CUI_Cursor.h"
#include "CUI.h"
#include "CInventory_Manager.h"
#include "CUI_ItemInfo.h"


USING(Client)

CUI_Window_Inventory::CUI_Window_Inventory(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice,pContext)
{
}

CUI_Window_Inventory::CUI_Window_Inventory(const CUI_Window_Inventory& rhs)
	: CGameObject(rhs)
{
}


CUI_Window_Inventory* CUI_Window_Inventory::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CUI_Window_Inventory* pInstance = new CUI_Window_Inventory(pDevice,pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Cloned : CUI_Window_Inventory");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Window_Inventory::Clone(void* pArg)
{
	return nullptr;
}



HRESULT CUI_Window_Inventory::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	m_pInput_Manager = CInput_Manager::GetInstance();
	m_InvenSlots.resize(maxIdx + 1);

	m_pInventory_Manager = CInventory_Manager::GetInstance();


    return S_OK;
}

HRESULT CUI_Window_Inventory::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;


	return S_OK;
}

void CUI_Window_Inventory::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CUI_Window_Inventory::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	bool m_bPressed = false;
	int iPreIdx = m_iCurIdx;

	if (m_pInput_Manager->IsKeyPressed(KeyCode::LeftArrow))
	{
		--m_iCurIdx;
		m_bPressed = true;
		m_pGameInstance->PlaySoundW(L"System/Hover.wav", CHANNELID::SOUND_UI, g_UIVolume);
	}

	if (m_pInput_Manager->IsKeyPressed(KeyCode::RightArrow))
	{
		++m_iCurIdx;
		m_bPressed = true;
		m_pGameInstance->PlaySoundW(L"System/Hover.wav", CHANNELID::SOUND_UI, g_UIVolume);
	}

	if (m_pInput_Manager->IsKeyPressed(KeyCode::UpArrow))
	{
		m_iCurIdx -= 4;
		m_bPressed = true;
		m_pGameInstance->PlaySoundW(L"System/Hover.wav", CHANNELID::SOUND_UI, g_UIVolume);
	}
	if (m_pInput_Manager->IsKeyPressed(KeyCode::DownArrow))
	{
		m_iCurIdx += 4;
		m_bPressed = true;
		m_pGameInstance->PlaySoundW(L"System/Hover.wav", CHANNELID::SOUND_UI, g_UIVolume);
	}

	if (m_pInput_Manager->IsKeyPressed(KeyCode::X))
	{
		Update_UpdateXSlot();
		m_bPressed = true;

	}



	
	if (m_bPressed)
	{
		m_iCurIdx = MathUtils::Clamp(m_iCurIdx, 0, maxIdx);
		
		Update_Cursor(iPreIdx);
	}
	

}

void CUI_Window_Inventory::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	
	
	
}

void CUI_Window_Inventory::Update_Cursor(int PreIdx)
{
	CheckTrue(m_InvenSlots.empty());

	CheckNull(m_InvenSlots[PreIdx]);
	CheckNull(m_InvenSlots[m_iCurIdx]);

	CInventorySlot* pPreSlot = m_InvenSlots[PreIdx];
	CInventorySlot* pSlot = m_InvenSlots[m_iCurIdx];

	const CUI::UIInfo* SlotInfo = pSlot->Get_UIInitInfo();
	 
	m_Cursor->Set_Pos(SlotInfo->fX, SlotInfo->fY, m_Cursor->Get_UIInitInfo()->Depth);
	m_Cursor->Set_SelectSlot(pSlot);
	
	pPreSlot->Change_State(CSlot::State::IDLE);
	pSlot->Change_State(CSlot::State::SELECT);

	//커서위치에 따른 UIInfo갱신
	Update_InventoryItemInfo();
}


void CUI_Window_Inventory::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}

HRESULT CUI_Window_Inventory::Render()
{
	__super::Render();
    return S_OK;
}



void CUI_Window_Inventory::Add_InvenSlots(CGameObject* pObj,int iIdx)
{
	CInventorySlot* pSlot = dynamic_cast<CInventorySlot*>(pObj);
	CheckNull(pSlot);

	int iTargetIdx = pSlot->Get_Idx();

	m_InvenSlots[iTargetIdx]=(pSlot);
	Safe_AddRef(pSlot);

	
}

void CUI_Window_Inventory::Set_Cursor(CGameObject* pObj)
{
	CUI_Cursor* pCursor = dynamic_cast<CUI_Cursor*>(pObj);
	CheckNull(pCursor);

	m_Cursor = pCursor;
	Safe_AddRef(m_Cursor);

	Update_Cursor(0);

}

void CUI_Window_Inventory::Set_ItemInfoUI(CGameObject* pObj)
{

	CUI_ItemInfo* pItemUI = dynamic_cast<CUI_ItemInfo*>(pObj);
	CheckNull(pItemUI);

	m_ItemInfoUI = pItemUI;
	Safe_AddRef(m_ItemInfoUI);

}

void CUI_Window_Inventory::Update_InventorySlots()
{
	vector<InvenSlot*>* AllInven = m_pInventory_Manager->Get_AllInven();
	CheckTrue(AllInven->empty());
	
	for (auto& pItemInfo : (*AllInven))
	{
		if (pItemInfo)
		{
			//이미 UI에 사용되고있다면.. FAlse
			if (pItemInfo->m_bUsedInUI)
				continue;
			for (auto& Slot : m_InvenSlots)
			{
				if (Slot->Is_HasItem())
					continue;

				else
				{
					Slot->Update_Item(pItemInfo->ItemType);
					pItemInfo->m_bUsedInUI = true;
					break;
				}
			}
		}
		
	}
	
}

void CUI_Window_Inventory::Update_InventoryItemInfo()
{
	CInventorySlot* pSlot = m_InvenSlots[m_iCurIdx];


	//커서위치에 따른 UIInfo갱신
	if (pSlot->Is_HasItem())
	{
		m_ItemInfoUI->Set_Active(true);
		m_ItemInfoUI->Update_ItemInfo(pSlot->Get_ItemType());

	}

	else
	{
		m_ItemInfoUI->Set_Active(false);
	}
}

void CUI_Window_Inventory::Update_OnUseItem()
{
	vector<InvenSlot*>* AllInven = m_pInventory_Manager->Get_AllInven();
	CheckTrue(AllInven->empty());

	for (auto& pItemInfo : (*AllInven))
	{
		if (pItemInfo)
		{
			//카운트가 0이하가아니라면
			if (pItemInfo->count <= 0 == false)
				continue;

			else
			{
				for (auto& Slot : m_InvenSlots)
				{
					if (Slot->Is_HasItem())
					{
						if (Slot->Get_ItemType() == pItemInfo->ItemType)
						{
							Slot->Update_Item(ItemType::END);
						}
					}

					else
						continue;

					if (m_pInventory_Manager->Get_XSlot())
					{
						if (m_pInventory_Manager->Get_XSlot()->ItemType == pItemInfo->ItemType)
							Clean_QUickSlot();
					}
					

					
				}
			}
			
		}

	}

}

void CUI_Window_Inventory::Update_UpdateXSlot()
{
	CheckNull(m_InvenSlots[m_iCurIdx]);
	CInventorySlot* pSlot = m_InvenSlots[m_iCurIdx];
	CheckTrue(pSlot->Get_ItemType() == ItemType::END);

	//x슬롯 처음설정하는거면, 바로설정
	if (m_pInventory_Manager->Get_XSlot() == nullptr)
	{
		if (pSlot->Is_HasItem())
		{
			pSlot->Set_QuickSlot(true);
					}
		else
			return;
	}

	else
	{
		CInventorySlot* pPreSlot = m_InvenSlots[m_iXIdx];
		if (pPreSlot)
			pPreSlot->Set_QuickSlot(false);

		pSlot->Set_QuickSlot(true);
	}

	
	m_iXIdx = pSlot->Get_Idx();
	m_pInventory_Manager->Set_SlotKey(pSlot->Get_ItemType(), KeyCode::X);
	m_pGameInstance->PlaySoundW(L"System/XSlot.wav", CHANNELID::SOUND_UI, g_UIVolume);

}

void CUI_Window_Inventory::Clean_QUickSlot()
{
	m_iXIdx = -1;
	m_pGameInstance->BroadCastEvent(L"CleanInvenSlotIcon",nullptr);

}


void CUI_Window_Inventory::Free()
{
	__super::Free();

	for (auto& pObj : m_InvenSlots)
		Safe_Release(pObj);

	Safe_Release(m_ItemInfoUI);
	Safe_Release(m_Cursor);

}

