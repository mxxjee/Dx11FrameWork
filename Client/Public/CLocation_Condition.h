#pragma once
#include "CQuestCondition.h"


/*visitedtrigger에 의해서 변수가 true가되면 완료된거.*/

NS_BEGIN(Client)
class CLocation_Condition :
    public CQuestCondition
{
public:
    CLocation_Condition();
    virtual ~CLocation_Condition();

public:
    void    Set_Visited(bool b) { bVisited = b; }

public:
    //매프레임마다 조건체크를 통해 true/false를반환
    virtual bool IsSatisfied();

private:
    bool bVisited = false;
};

NS_END

