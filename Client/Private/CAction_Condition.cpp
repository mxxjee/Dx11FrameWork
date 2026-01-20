#include "CAction_Condition.h"
#include "CPlayer.h"

USING(Client)
CAction_Condition::CAction_Condition()
{
}

CAction_Condition::CAction_Condition(class CPlayer* pPlayer, _uint iPlayerState)
    :CQuestCondition()
{
    CheckNull(pPlayer);
    m_pPlayer = pPlayer;

    m_iCheckPlayerState = iPlayerState;

}

CAction_Condition::~CAction_Condition()
{
}

bool CAction_Condition::IsSatisfied()
{
    CheckNullResult(m_pPlayer,false);
    if (m_pPlayer->Get_State() == m_iCheckPlayerState)
        return true;

    return false;
}
