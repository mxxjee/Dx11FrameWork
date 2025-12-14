#include "ItemCollect_Condition.h"

USING(Client)
ItemCollect_Condition::ItemCollect_Condition()
{
}

ItemCollect_Condition::ItemCollect_Condition(_uint iCollectItemID, int iCount)
{
    m_iCount = iCount;
    m_iCheckItem = iCollectItemID;

}

ItemCollect_Condition::~ItemCollect_Condition()
{
}

bool ItemCollect_Condition::IsSatisfied()
{
    /*if (m_pInventoryManager->HasItem(itemID))
        return true;

    return false;*/


    return false;

}
