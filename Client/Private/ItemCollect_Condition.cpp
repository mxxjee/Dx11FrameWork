#include "ItemCollect_Condition.h"
#include "CInventory_Manager.h"


USING(Client)
ItemCollect_Condition::ItemCollect_Condition()
    :m_pInventory_Manager(CInventory_Manager::GetInstance())
{
}

ItemCollect_Condition::ItemCollect_Condition(ItemType iCollectItemID, int iCount)
{
    m_iCount = iCount;
    m_iCheckItem = iCollectItemID;
    m_pInventory_Manager = CInventory_Manager::GetInstance();
}

ItemCollect_Condition::~ItemCollect_Condition()
{
}

bool ItemCollect_Condition::IsSatisfied()
{
    if (m_pInventory_Manager->Find_Inven(m_iCheckItem))
        return true;


    return false;

}
