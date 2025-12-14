#pragma once
#include "Client_Defines.h"

NS_BEGIN(Client)
class CQuestCondition
{

public:
    CQuestCondition();
    virtual ~CQuestCondition();


public:
            //매프레임마다 조건체크를 통해 true/false를반환
    virtual bool IsSatisfied() = 0;
};
NS_END
