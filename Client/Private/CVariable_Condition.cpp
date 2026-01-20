#include "CVariable_Condition.h"

USING(Client)
CVariable_Condition::CVariable_Condition()
{
}

CVariable_Condition::CVariable_Condition(CQuest_Manager::QuestFact eType, bool bTargetValue)
    :m_iCheckFact(eType),
    m_bTargetValue(bTargetValue),
    m_pQuest_Manager(CQuest_Manager::GetInstance())
{
}



CVariable_Condition::~CVariable_Condition()
{
}

bool CVariable_Condition::IsSatisfied()
{
    if (m_pQuest_Manager->Get_FactCheckValue(m_iCheckFact) == m_bTargetValue)
        return true;

    else
        return false;

    return false;

}
