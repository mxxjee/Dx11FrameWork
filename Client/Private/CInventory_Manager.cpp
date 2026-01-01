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

void CInventory_Manager::Update(_float fTimeDelta)
{
	//큐를검사하면서 진짜 넣을수있는지 체크
	CheckTrue(m_SlotQueue.empty());

	InvenStanby* pFront = m_SlotQueue.front();
	if (pFront->m_bEnd)
	{
		InvenSlot* pNewSlot = new InvenSlot;
		pNewSlot->ItemType = pFront->ItemType;
		pNewSlot->count = pFront->count;
		pNewSlot->m_pItemInfo = pFront->m_pItem;

		m_InvenSlots[ENUM_TO_UINT(pNewSlot->ItemType)] = pNewSlot;


		//아이템마다 다름. 얻은이후 실행할 이벤트전송
		if (pFront->m_pItem->m_AfterGetFunc)
			pFront->m_pItem->m_AfterGetFunc();

		//uiscene 갱신
		if (Update_UISCene)
			Update_UISCene();


		m_SlotQueue.pop_front();
		Safe_Delete(pFront);
	}

	else
	{
		//아ㅣ나렴ㄴ UI요청해라. 매프레임불려도 같은상태라면 안바뀜.
		Request_UI_Event(pFront->ItemType);
	}



}
bool CInventory_Manager::Request_Add_To_Inven(ItemType itemType, int iCount)
{
	/*인벤 대기열에 넣을 거 관리*/
	CheckTrueResult(iCount == 0,false);

	ITMINFO* pItemInfo = m_pItemManager->Get_ItemInfo(itemType);
	CheckNullResult(pItemInfo, false);
	InvenSlot* pSlot = Find_Inven(pItemInfo->ItemType);
	
	//이미대기열에존재한다면, 추가 X(1개씩얻어야하는 아이템들)
	InvenStanby* pFindStanby = Find_Stanby(pItemInfo->ItemType);
	CheckTrueResult(pFindStanby!=nullptr, false);

	InvenStanby* pStanBy = new InvenStanby;
	pStanBy->ItemType = pItemInfo->ItemType;
	pStanBy->m_pItem = m_pItemManager->Get_ItemInfo(itemType);




	//같은아이템 또먹은경우
	if (pSlot)
		pStanBy->count += iCount + pSlot->count;

	else
	{
		pStanBy->count = iCount;
		m_SlotQueue.push_back(pStanBy);

	}


	

	return true;
}

InvenSlot* CInventory_Manager::Find_Inven(ItemType eType)
{
	CheckTrueResult(m_InvenSlots.empty(), nullptr);
	return m_InvenSlots[ENUM_TO_UINT(eType)];
}

void CInventory_Manager::Free()
{
	for (auto& pSlot : m_InvenSlots)
		Safe_Delete(pSlot);
	
	for (auto& pSlot : m_SlotQueue)
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

void CInventory_Manager::Set_End_in_SlotQueue(ItemType eType, bool b)
{
	for (auto& slot : m_SlotQueue)
	{
		if (slot->ItemType == eType)
		{
			slot->m_bEnd = b;
			return;

		}
	}
}

void CInventory_Manager::Request_UI_Event(ItemType eType)
{
	//UI활성화..
	//아이템원본 정보가져오기
	//이미 아이템얻고있는상태라면 빠꾸
	if (!m_pPlayer)
		m_pPlayer = CGameManager::GetInstance()->Get_MainPlayer();

	CheckNull(m_pPlayer);
	CheckTrue(m_pPlayer->Get_ActionControl()->m_bItemGet);

	ITMINFO* pOriginITMINFO = m_pItemManager->Get_ItemInfo(eType);
	CheckNull(pOriginITMINFO);
	CheckNull(m_ItemEvent);

	m_ItemEvent->eType = eType;
	m_ItemEvent->ItemDesc = pOriginITMINFO->ItemDesc;
	m_ItemEvent->TexKey = pOriginITMINFO->TexKey;
	m_ItemEvent->OffSet = _float3(0.f, -130.f, 0.f);


	//플레이어한테 애니메이션 실행시켜라.
	//플레이어는 여기 애님노티파이를 통해서 ui활성화
	



	//아이템얻은상태로 변경
	m_pPlayer->Set_GetItem(true);



}

CInventory_Manager::InvenStanby* CInventory_Manager::Find_Stanby(ItemType eType)
{
	CheckTrueResult(m_SlotQueue.empty(),nullptr);
	for (auto& pDst : m_SlotQueue)
	{
		if (pDst->ItemType == eType)
			return pDst;
	}

	return nullptr;
}

bool CInventory_Manager::Use_Item(ItemType eType, int iCount)
{
	InvenSlot* pSlot = m_InvenSlots[ENUM_TO_UINT(eType)];
	CheckNullResult(pSlot, false);
	CheckTrueResult(pSlot->count < iCount,false);

	ITMINFO* pItem = pSlot->m_pItemInfo;
	if (pItem->m_bCanUse)
		pSlot->count -= iCount;

	else
		return true;

	//사용시 플레이ㅓㅇ상태를 제어한다면
	int iState = pItem->PlayerState;
	if (iState != -1)
	{
		CPlayer* pPlayer = CGameManager::GetInstance()->Get_MainPlayer();
		CheckNullResult(pPlayer, false);
		pPlayer->Change_State(iState);
	}

	if (pSlot->count <= 0)
	{


		if (Update_OnUseItem)
			Update_OnUseItem();


		if (m_XSlot == pSlot)
			m_XSlot = nullptr;

		Safe_Delete(pSlot);
		pSlot = nullptr;
	}

	

	return true;

}

void CInventory_Manager::Set_SlotKey(ItemType eType, KeyCode code)
{
	InvenSlot* pSlot = Find_Inven(eType);
	CheckNull(pSlot);
	
	ITMINFO* pInfo = m_pItemManager->Get_ItemInfo(eType);
	CheckNull(pInfo);
	CheckFalse(pInfo->PlayerState);

	if (code == KeyCode::X)
	{
		//이미있으면 교체해야함/ 키바꾸기
		if (m_XSlot)
		{
			m_XSlot->m_eQuickKeyCode = KeyCode::None;
		}
	}

	

	/////////모두 맞다면///////
	/*퀵슬롯 지정*/
	pSlot->m_eQuickKeyCode = code;

	if (code == KeyCode::X)
		m_XSlot = pSlot;


	//UI Event
	CGameInstance::GetInstance()->BroadCastEvent(L"UpdateInvenSlotIcon", &pInfo->TexKey);
}

bool CInventory_Manager::Use_QuickSlot_Item(KeyCode e, int _iCount)
{
	CheckTrueResult(m_XSlot==nullptr, false);

	return Use_Item(m_XSlot->ItemType, _iCount);
}
