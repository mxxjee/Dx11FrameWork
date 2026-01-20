#pragma once
#include "CQuestCondition.h"

/*인벤토리매니저한테 요청해서 있는지확인하다.*/
NS_BEGIN(Client)
class CInventory_Manager;

class ItemCollect_Condition :
    public CQuestCondition
{


public:
    ItemCollect_Condition();
    ItemCollect_Condition(ItemType iCollectItemID, int iCount);
    virtual ~ItemCollect_Condition();


public:
    //매프레임마다 조건체크를 통해 true/false를반환
    virtual bool IsSatisfied();


private:
    int         m_iCount = 0;
    ItemType       m_iCheckItem;

    class CInventory_Manager* m_pInventory_Manager = nullptr;

};

NS_END