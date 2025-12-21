#pragma once
#include "CBase.h"
#include "Client_Defines.h"



NS_BEGIN(Client)
class CItem_Manager;
class CPlayer;


class CInventory_Manager :
    public CBase
{
public:

    DECLARE_SINGLETON(CInventory_Manager)

public:
            //아이템 얻는 UI띄울떄 보낼 구조체
    struct ItemGetEvent
    {
        wstring TexKey = L"";   //어떤아이템꺼 띄울건지
        _float3 OffSet = _float3(0.f, 0.f, 0.f);        //오프셋값
        wstring ItemDesc = L"";

    };
private:
    explicit CInventory_Manager();
    virtual ~CInventory_Manager()=default;


public:
    //아이템소유중인지체크 
    InvenSlot* Find_Inven(ItemType eType);
    CInventory_Manager::ItemGetEvent* Get_ItemGetEvent();
    int             Get_InvenSize();    //인벤토리 아이템 총 개수 판단
public:
    bool         Add_To_Inven(ItemType eType, int iCount);
                
   

public:
    void            Free() override;

private:
    vector<InvenSlot*>     m_InvenSlots;
    CItem_Manager* m_pItemManager = nullptr;

private:
            //UI이벤트때 전송할 데이터들
    CInventory_Manager::ItemGetEvent* m_ItemEvent = nullptr;        //가장 최근 얻은 아이템정보

private:
    CPlayer* m_pPlayer = nullptr;


};
NS_END

