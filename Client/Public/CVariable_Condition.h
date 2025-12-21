#pragma once
#include "CQuestCondition.h"
#include "CQuest_Manager.h"

NS_BEGIN(Client)
class CVariable_Condition :
    public CQuestCondition
{
public:
    CVariable_Condition();
    CVariable_Condition(CQuest_Manager::QuestFact eType,bool bTargetValue=true);
    virtual ~CVariable_Condition();

public:
    virtual bool IsSatisfied() override;

private:
    class CPlayer* m_pPlayer = nullptr;
    CQuest_Manager::QuestFact       m_iCheckFact = CQuest_Manager::QuestFact::END;        //체크해야하는 퀘스트fact
    bool        m_bTargetValue = false;
    
    CQuest_Manager*         m_pQuest_Manager = nullptr;

};

NS_END