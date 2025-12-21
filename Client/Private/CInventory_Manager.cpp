#include "CInventory_Manager.h"
#include "CItem_Manager.h"
#include "CGameManager.h"
#include "CPlayer.h"



USING(Client)

IMPLEMENT_SINGLETON(CInventory_Manager)


CInventory_Manager::CInventory_Manager()
{
	m_InvenSlots.resize(ENUM_TO_UINT(ItemType::END));
	m_pItemManager= CItem_Manager::GetInstance();


	m_ItemEvent = new CInventory_Manager::ItemGetEvent();

}

bool CInventory_Manager::Add_To_Inven(ItemType eType, int iCount)
{
	CheckTrueResult(iCount == 0,false);

	InvenSlot* pSlot = Find_Inven(eType);
	if (pSlot)
	{
		pSlot->count += iCount;
	}

	else
	{
		InvenSlot* pNewSlot = new InvenSlot;

		pNewSlot->ItemType = eType;
		pNewSlot->count = iCount;
		m_InvenSlots[ENUM_TO_UINT(eType)] = pNewSlot;
	}


	//UI활성화..
	//아이템원본 정보가져오기
	ITMINFO* pOriginITMINFO = m_pItemManager->Get_ItemInfo(eType);
	CheckNullResult(pOriginITMINFO,false);
	CheckNullResult(m_ItemEvent, false);

	m_ItemEvent->ItemDesc = pOriginITMINFO->ItemDesc;
	m_ItemEvent->TexKey = pOriginITMINFO->TexKey;
	m_ItemEvent->OffSet = _float3(0.f, -200.f, 0.f);


	//플레이어한테 애니메이션 실행시켜라.
	//플레이어는 여기 애님노티파이를 통해서 ui활성화
	if (!m_pPlayer)
		m_pPlayer = CGameManager::GetInstance()->Get_MainPlayer();


	CheckNullResult(m_pPlayer, false);

	//아이템얻은상태로 변경
	m_pPlayer->Set_GetItem(true);




	return true;
}

InvenSlot* CInventory_Manager::Find_Inven(ItemType eType)
{
	return m_InvenSlots[ENUM_TO_UINT(eType)];
}

void CInventory_Manager::Free()
{
	for (auto& pSlot : m_InvenSlots)
		Safe_Delete(pSlot);

	Safe_Delete(m_ItemEvent);

}


CInventory_Manager::ItemGetEvent* CInventory_Manager::Get_ItemGetEvent()
{
	return m_ItemEvent;
}

int CInventory_Manager::Get_InvenSize()
{
	_uint Size = 0;
	for (_uint i = 0; i < ENUM_TO_UINT(ItemType::END); ++i)
	{
		if (m_InvenSlots[i] != nullptr)
		{
			for (int j = 0; j < m_InvenSlots[i]->count; ++j)
				++Size;

		}
			
	}
	return Size;
}
