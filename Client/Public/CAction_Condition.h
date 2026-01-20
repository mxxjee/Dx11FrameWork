#pragma once
#include "CQuestCondition.h"

NS_BEGIN(Client)
class CAction_Condition :
    public CQuestCondition
{
public:
    CAction_Condition();
    CAction_Condition(class CPlayer* pPlayer, _uint iPlayerState);
    virtual ~CAction_Condition();

public:
    virtual bool IsSatisfied() override;

private:
    class CPlayer* m_pPlayer = nullptr;
    _uint       m_iCheckPlayerState = 0;        //체크해야하는 플레이어상태
};
NS_END

