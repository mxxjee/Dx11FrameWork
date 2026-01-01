#pragma once
#include "CBase.h"
#include "Client_Defines.h"



NS_BEGIN(Client)
class CItem_Manager;
class CPlayer;

/*바로인벤에 넣어지는것이아니ㅏㄹ 여러개의 아이템을 동시에 얻었을떄
하나씩 처리되도록 설정*/

class CInventory_Manager :
    public CBase
{
public:

    DECLARE_SINGLETON(CInventory_Manager)

public:
            //아이템 얻는 UI띄울떄 보낼 구조체
    struct ItemGetEvent
    {
        ItemType eType = ItemType::END;
        wstring TexKey = L"";   //어떤아이템꺼 띄울건지
        _float3 OffSet = _float3(0.f, 0.f, 0.f);        //오프셋값
        wstring ItemDesc = L"";

    };

    struct InvenStanby
    {
        ItemType	ItemType = ItemType::END;
        int			count = 0;
        bool        m_bEnd = false;
        ITMINFO*    m_pItem = nullptr;

 
    };
private:
    explicit CInventory_Manager();
    virtual ~CInventory_Manager()=default;

public:
    void        Update(_float fTimeDelta);

public:
    //아이템소유중인지체크 
    InvenSlot* Find_Inven(ItemType eType);
    InvenStanby* Find_Stanby(ItemType eType);

    //퀵슬롯아이템사용하기
    bool        Use_QuickSlot_Item(KeyCode e,int _iCount=1);

    CInventory_Manager::ItemGetEvent* Get_ItemGetEvent();
    int             Get_InvenSize();    //인벤토리 아이템 총 개수 판단
    void            Set_SlotKey(ItemType eType,KeyCode code);
    vector<InvenSlot*>* Get_AllInven() { return &m_InvenSlots; }
public:

                //이 아이템 인벤에 넣을거니까 유아이 띄워.
    bool         Request_Add_To_Inven(ItemType itemType, int iCount);
    bool        Use_Item(ItemType eType, int iCount);

    void         Request_UI_Event(ItemType eType);
                    //Desc사라지고 이후에 true로만들어서 이후에 인벤에 진짜추가됨
    void         Set_End_in_SlotQueue(ItemType eType, bool b);
   
    void        Set_UpdateUISceneEvent(std::function<void()> Func) { Update_UISCene = Func; }
    void        Set_Update_OnUseItem(std::function<void()> Func) { Update_OnUseItem = Func; }
public:
    void            Free() override;

private:
    vector<InvenSlot*>     m_InvenSlots;
    CItem_Manager* m_pItemManager = nullptr;

private:
            //UI이벤트때 전송할 데이터들
    CInventory_Manager::ItemGetEvent* m_ItemEvent = nullptr;        //가장 최근 얻은 아이템정보

private:
    deque<InvenStanby*>      m_SlotQueue;
    CPlayer* m_pPlayer = nullptr;


private:
    InvenSlot*        m_XSlot = nullptr;//X키눌렀을때의 아이템(퀵 슬롯:X)
    InvenSlot*        m_YSlot = nullptr;//Y키눌렀을때의 아이템(퀵 슬롯:X)


private:
    std::function<void()> Update_UISCene = nullptr;
    std::function<void()> Update_OnUseItem = nullptr;
};
NS_END

